#ifndef PULL_STAR_BENCH_REGEX_BENCH_H_
#define PULL_STAR_BENCH_REGEX_BENCH_H_

#include "benchmark.h"
#include "regex.h"

#include "text/text_index.h"
#include "Aggregator.h"

namespace pull_star_bench {

class RegExBench : public dsl_bench::Benchmark {
 public:
  RegExBench(const std::string& input_file, bool construct, int data_structure,
             int executor_type);

  RegExBench(int executor_type);

  void benchRegex(const std::string& query_file,
                  const std::string& result_path);

  void benchRegex(pull_star_thrift::AggregatorClient& client,
                  const std::string& query_file,
                  const std::string& result_path);

  void benchSearch(const std::string& query_file,
                  const std::string& result_path);

  void benchSearch(pull_star_thrift::AggregatorClient& client,
                  const std::string& query_file,
                  const std::string& result_path);

  void benchBreakdown(const std::string& query_file,
                      const std::string& result_path);

 private:
  dsl::TextIndex *text_idx_;
  pull_star::RegularExpression::ExecutorType executor_type_;
};

}

#endif // PULL_STAR_BENCH_REGEX_BENCH_H_
