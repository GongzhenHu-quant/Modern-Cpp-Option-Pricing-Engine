#include "Models.h"
#include <algorithm>
#include <numeric>

//实现看涨期权 
CallPayoff::CallPayoff(double strike) : strike_(strike) {}

double CallPayoff::operator()(double spot) const {
	return std::max(spot - strike_ , 0.0);
}

//实现看跌期权 
PutPayoff::PutPayoff(double strike) : strike_(strike) {}

double PutPayoff::operator()(double spot) const {
	return std::max(strike_ - spot, 0.0);
}

//实现欧式期权 
EuropeanOption::EuropeanOption(double time, std::shared_ptr<Payoff> payoff_ptr):
	expiry_(time), payoff_ptr_(std::move(payoff_ptr)) { }

std::vector<double> EuropeanOption::getObservationTimes() const {
	return { expiry_ };
}

double EuropeanOption::calculatePayoff(const std::vector<double>& path) const {
	return (*payoff_ptr_)(path.back());
}

//实现亚式期权 
AsianOption::AsianOption(const std::vector<double>& times, std::shared_ptr<Payoff> payoff_ptr) :
	obsTimes_(times), payoff_ptr_(std::move(payoff_ptr)) { }

std::vector<double> AsianOption::getObservationTimes() const {
	return obsTimes_;
}

double AsianOption::calculatePayoff(const std::vector<double>& path)const {
	double sum = std::accumulate(path.begin(), path.end(), 0.0);
	double average = sum / path.size();
	return (*payoff_ptr_)(average);
}