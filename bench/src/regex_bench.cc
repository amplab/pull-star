#include "regex_bench.h"

#include <unistd.h>
#include <cstdlib>
#include <cstdio>

#include "regex.h"
#include "suffix_tree.h"
#include "compressed_suffix_tree.h"

pull_star_bench::RegExBench::RegExBench(const std::string& input_file,
                                        bool construct, int data_structure,
                                        int executor_type)
    : dsl_bench::Benchmark() {
  if (construct) {
    std::ifstream input_stream(input_file);
    const std::string input_text((std::istreambuf_iterator<char>(input_stream)),
                                 std::istreambuf_iterator<char>());
    input_stream.close();
    if (data_structure == 0) {
      text_idx_ = new dsl::SuffixTree(input_text);

      // Serialize to disk for future use.
      std::ofstream out(input_file + ".st");
      text_idx_->serialize(out);
      out.close();
    } else if (data_structure == 1) {
      text_idx_ = new dsl::CompressedSuffixTree(input_text, input_file);
    } else {
      fprintf(stderr, "Data structure %d not supported yet.\n", data_structure);
      exit(0);
    }
  } else {
    if (data_structure == 0) {
      std::ifstream input_stream(input_file + ".st");
      text_idx_ = new dsl::SuffixTree();
      text_idx_->deserialize(input_stream);
      input_stream.close();
    } else if (data_structure == 1) {
      std::ifstream input_stream(input_file);
      const std::string input_text(
          (std::istreambuf_iterator<char>(input_stream)),
          std::istreambuf_iterator<char>());
      text_idx_ = new dsl::CompressedSuffixTree(input_text, input_file, false);
      input_stream.close();
    }
  }
  executor_type_ =
      static_cast<pull_star::RegularExpression::ExecutorType>(executor_type);
}

void pull_star_bench::RegExBench::benchRegex(const std::string& query_file,
                                             const std::string& result_path) {
  std::vector<std::string> queries = readQueryFile(query_file);
  std::ofstream result_stream(result_path);
  for (auto query : queries) {
    pull_star::RegularExpression regex(query, text_idx_, executor_type_);
    std::set<std::pair<size_t, size_t>> results;
    time_t start = get_timestamp();
    regex.execute();
    regex.getResults(results);
    time_t end = get_timestamp();
    time_t tot = end - start;
    result_stream << results.size() << "\t" << tot << "\n";
  }
}

void print_usage(char *exec) {
  fprintf(
      stderr,
      "Usage: %s [-m mode] [-t type] [-q query_file] [-r res_file] [-d data_structure] [-e executor_type] [file]\n",
      exec);
}

int main(int argc, char **argv) {
  if (argc < 2 || argc > 14) {
    print_usage(argv[0]);
    return -1;
  }

  int c;
  bool construct = true;
  std::string type = "latency-regex";
  std::string query_file = "queries.txt";
  std::string res_file = "res.txt";
  int data_structure = 0;
  int executor_type = 1;

  while ((c = getopt(argc, argv, "m:t:q:r:d:e:")) != -1) {
    switch (c) {
      case 'm': {
        construct = atoi(optarg);
        break;
      }
      case 't': {
        type = std::string(optarg);
        break;
      }
      case 'q': {
        query_file = std::string(optarg);
        break;
      }
      case 'r': {
        res_file = std::string(optarg);
        break;
      }
      case 'd': {
        data_structure = atoi(optarg);
        break;
      }
      case 'e': {
        executor_type = atoi(optarg);
        break;
      }
      default: {
        fprintf(stderr, "Unsupported option %c.\n", (char) c);
        exit(0);
      }
    }
  }

  if (optind == argc) {
    print_usage(argv[0]);
    return -1;
  }

  std::string input_file = std::string(argv[optind]);
  pull_star_bench::RegExBench bench(input_file, construct, data_structure,
                                    executor_type);

  if (type == "latency-regex") {
    bench.benchRegex(query_file, res_file);
  } else {
    fprintf(stderr, "Unsupported type %s.\n", type.c_str());
    exit(0);
  }

  return 0;
}
