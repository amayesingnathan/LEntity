#include "fmt/format.h"

#include "Benchmark.h"

void RunBenchmark(size_t entityCount)
{
	LEnt::Benchmark benchmark(fmt::format("output/results{0}.json", entityCount));
	benchmark.run(entityCount);
}

int main()
{
	RunBenchmark(2048);

}