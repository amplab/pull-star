namespace cpp pull_star_thrift

service Shard {
    set<i64> regexSearch(1:string query),
    i32 init()
}
