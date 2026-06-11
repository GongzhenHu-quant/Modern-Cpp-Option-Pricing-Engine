#pragma once
#include <vector>
#include <memory>

//数据包 
struct MarketContext
{
	double spot;
	double vol;
	double rate;
};

//Payoff类 
class Payoff
{
public:
	[[nodiscard]] virtual double operator()(double spot) const = 0;
	virtual ~Payoff() = default;

};

//看涨期权 
class CallPayoff: public Payoff
{
public:
	explicit CallPayoff(double strike);

	[[nodiscard]] double operator()(double spot) const override;

private:
	double strike_;
};

//看跌期权 
class PutPayoff : public Payoff
{
public:
	explicit PutPayoff(double strike);

	[[nodiscard]] double operator()(double spot) const override;

private:
	double strike_;
};


//期权类 
class Instruments
{
public:
	virtual ~Instruments() = default;
	[[nodiscard]] virtual std::vector<double> getObservationTimes() const = 0;
	[[nodiscard]] virtual double calculatePayoff(const std::vector<double>& path) const = 0;

};

//欧式期权 
class EuropeanOption : public Instruments
{
public:
	EuropeanOption(double time, std::shared_ptr<Payoff> payoff_ptr);
	[[nodiscard]] std::vector<double> getObservationTimes() const override;
	[[nodiscard]] double calculatePayoff(const std::vector<double>& path) const override;

private:
	double expiry_;
	std::shared_ptr<Payoff> payoff_ptr_;

};

//亚式期权 
class AsianOption : public Instruments
{
public:
	AsianOption(const std::vector<double>& times, std::shared_ptr<Payoff> payoff_ptr);
	[[nodiscard]] std::vector<double> getObservationTimes() const override;
	[[nodiscard]] double calculatePayoff(const std::vector<double>& path) const override;

private:
	std::vector<double> obsTimes_;
	std::shared_ptr<Payoff> payoff_ptr_;

};