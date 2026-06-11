#pragma once
#include "Models.h" // 需要用到 Instrument 和 MarketContext 
#include <vector>
#include <utility>
#include <random>

//统计数据收集器（尾部风险）
class RiskStatistics
{
public:
	void addResult(double result);
	[[nodiscard]] double getMean() const;
	void reserve(size_t capacity);   //预留足够空间优化性能 
	[[nodiscard]] std::pair<double, double> getTailRisk(double confidenceLevel);

private:
	std::vector<double> payoff_history_;
};

//蒙特卡罗模拟引擎 
class MonteCarloEngine {
private:
	unsigned long numPaths;
	std::mt19937_64 rng;   //梅森旋转算法生成随机数 
	std::normal_distribution<double> normDist;  //生成正态分布 

public:
	explicit MonteCarloEngine(unsigned long Paths);
	void resetSeed(unsigned int seed) { rng.seed(seed); }
	//核心计算程序 
	void calculate(const Instruments& option, const MarketContext& mkt, RiskStatistics& stats);
};

//计算希腊字母
struct GreekResults {
	double price;
	double delta;
	double gamma;
};

class GreekCalculator {
private: 
	unsigned long numPaths_;
public:
	explicit GreekCalculator(unsigned long paths);

	//核心引擎，传入期权与当前市场数据，输出希腊字母结果
	GreekResults calculateGreeks(const Instruments& option, const MarketContext& mkt);
};