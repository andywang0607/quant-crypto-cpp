set(targets QuoteNode QuoteApi BybitQuote BinanceQuote)
foreach(target ${targets})
  add_subdirectory(${target})
endforeach(target)
