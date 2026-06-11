#include "Engine.h"
#include <algorithm>
#include <numeric>
#include <cmath>

//实现尾部风险的数据收集 
void RiskStatistics::addResult(double result) {
	payoff_history_.push_back(result);
}

double RiskStatistics::getMean() const {
	if (payoff_history_.empty()) return 0.0;
	double sum = std::accumulate(payoff_history_.begin(), payoff_history_.end(), 0.0);
	return sum / payoff_history_.size();
}

void RiskStatistics::reserve(size_t capacity) { payoff_history_.reserve(capacity); }

std::pair<double, double> RiskStatistics::getTailRisk(double confidencelevel) {
	if (payoff_history_.empty()) return { 0.0, 0.0 };

	std::vector<double> sortedpayoff_history = payoff_history_;

	size_t varIndex = static_cast<size_t>(sortedpayoff_history.size() * confidencelevel);		//计算置信度以下的数据指标 
	if (varIndex >= sortedpayoff_history.size()) varIndex = sortedpayoff_history.size() - 1;

	std::nth_element(sortedpayoff_history.begin(), sortedpayoff_history.begin() + varIndex, sortedpayoff_history.end());

	double var = sortedpayoff_history[varIndex];

	double tailSum = 0.0;
	size_t tailCount = sortedpayoff_history.size() - varIndex;
	for (size_t i = varIndex; i < sortedpayoff_history.size(); ++i) {
		tailSum += sortedpayoff_history[i];
	}
	double cvar = (tailCount > 0) ? (tailSum / tailCount) : var;

	return { var, cvar };
}

//实现蒙特卡罗模拟 
MonteCarloEngine::MonteCarloEngine(unsigned long paths)
	: numPaths(paths), rng(std::random_device{}()), normDist(0.0, 1.0) {
}

void MonteCarloEngine::calculate(const Instruments& option, const MarketContext& mkt, RiskStatistics& stats) {
	auto times = option.getObservationTimes();
	if (times.empty()) return;

	stats.reserve(numPaths);
	size_t steps = times.size();
	std::vector<double> dt(steps);		//时间间隔 
	std::vector<double> drift(steps);	//漂移项 
	std::vector<double> volSqrtDt(steps);	//波动项 

	double currentTime = 0.0;
	for (size_t i = 0; i < steps; ++i) {
		dt[i] = times[i] - currentTime;
		drift[i] = (mkt.rate - 0.5 * mkt.vol * mkt.vol) * dt[i];
		volSqrtDt[i] = mkt.vol * std::sqrt(dt[i]);
		currentTime = times[i];
	}

	std::vector<double> path(steps);
	double discount = std::exp(-mkt.rate * times.back()); //折现因子 

	for (unsigned long p = 0; p < numPaths; ++p) {
		double currentSpot = mkt.spot;
		for (size_t i = 0; i < steps; ++i) {
			double Z = normDist(rng);
			currentSpot *= std::exp(drift[i] + volSqrtDt[i] * Z);
			path[i] = currentSpot;
		}
		double PayoffVal = option.calculatePayoff(path);
		stats.addResult(PayoffVal * discount);
	}
}

GreekCalculator::GreekCalculator(unsigned long paths) : numPaths_(paths) {};

//利用差分方法计算希腊字母敏感性 
GreekResults GreekCalculator::calculateGreeks(const Instruments& option, const MarketContext& mkt) {
	MonteCarloEngine engine(numPaths_);
	unsigned int commonSeed = 729; // 固定随机数种子 

	// 1. 计算基准价格 (Base Price) 
	engine.resetSeed(commonSeed);
	RiskStatistics baseStats;
	engine.calculate(option, mkt, baseStats);
	double p_base = baseStats.getMean();

	//考虑价格百分之一的微小扰动 
	double h = mkt.spot * 0.01;

	//标的资产价格上涨
	MarketContext mkt_up = mkt;
	mkt_up.spot = mkt.spot + h;
	engine.resetSeed(commonSeed);
	RiskStatistics upStats;
	engine.calculate(option, mkt_up, upStats);
	double p_up = upStats.getMean();

	// 标的资产价格下跌 
	MarketContext mkt_down = mkt;
	mkt_down.spot = mkt.spot - h;
	engine.resetSeed(commonSeed);
	RiskStatistics downStats;
	engine.calculate(option, mkt_down, downStats);
	double p_down = downStats.getMean();

	// 一阶中心差分计算 Delta
	double delta = (p_up - p_down) / (2.0 * h);
	// 二阶中心差分计算 Gamma
	double gamma = (p_up - 2.0 * p_base + p_down) / (h * h);

	return { p_base, delta, gamma };
}