#include <iostream>
#include <fstream>
#include "Models.h"
#include "Engine.h"

int main(){
	MarketContext mkt{ 200.0, 0.25, 0.075 };

	auto callpayoff_ptr = std::make_shared<CallPayoff>(200.0);
	EuropeanOption euroOpt(1.0, callpayoff_ptr);

	std::vector<double> asianTimes;
	for (int i = 1; i <= 12; ++i) asianTimes.push_back(i / 12.0);
	AsianOption asiaOpt(asianTimes, callpayoff_ptr);

	// 导出期权价格与风险对比可视化的CSV文件
	std::ofstream csvFile("comparison_results.csv");
	if (!csvFile.is_open()) {
		std::cerr << "Error: Could not create data file.\n";
		return 1;
	}

	csvFile << "Spot,EuroPrice,EuroVaR,EuroCVaR,AsianPrice,AsianVaR,AsianCVaR\n";

	std::cout << "Engine is scanning Spot Prices (150-250) and extracting all Risk Metrics...\n";

	MonteCarloEngine engine(100000);

	for (double current_spot = 150.0; current_spot <= 250.0; current_spot += 2.0) {
		MarketContext dynamic_mkt = mkt;
		dynamic_mkt.spot = current_spot;

		// 实行欧式期权的模拟 
		RiskStatistics euroStats;
		engine.calculate(euroOpt, dynamic_mkt, euroStats);
		double euroPrice = euroStats.getMean();
		auto [euroVaR, euroCVaR] = euroStats.getTailRisk(0.95); //置信度为0.95 

		// 实行亚式期权的模拟 
		RiskStatistics asiaStats;
		engine.calculate(asiaOpt, dynamic_mkt, asiaStats);
		double asianPrice = asiaStats.getMean();
		auto [asianVaR, asianCVaR] = asiaStats.getTailRisk(0.95); //置信度为0.95 

		//在CSV文件中写入两种期权的价格以及风险数据 
		csvFile << current_spot << ","
			<< euroPrice << "," << euroVaR << "," << euroCVaR << ","
			<< asianPrice << "," << asianVaR << "," << asianCVaR << "\n";
	}

	csvFile.close();
	std::cout << "Full metrics data generation completed! 'comparison_results.csv' is ready.\n\n";

	// 导出希腊字母敏感性可视化的CSV文件  
	std::ofstream csvFile_greeks("greeks_sensitivity.csv");
	if (!csvFile_greeks.is_open()) {
		std::cerr << "Error: Could not create data file.\n";
		return 1;
	}

	csvFile_greeks << "Spot,EuroDelta,EuroGamma,AsianDelta,AsianGamma\n";

	std::cout << "Engine is scanning Spot Prices (150-250) and extracting all Greeks...\n";


	GreekCalculator commander(200000);
	for (double current_spot_1 = 150; current_spot_1 <= 250; current_spot_1 += 2.0) {
		MarketContext dynamic_mkt_1 = mkt;
		dynamic_mkt_1.spot = current_spot_1;

		//计算希腊字母敏感性 
		GreekResults resultsasia = commander.calculateGreeks(asiaOpt, dynamic_mkt_1);
		GreekResults resultseuro = commander.calculateGreeks(euroOpt, dynamic_mkt_1);
		//在CSV文件中写入两种期权的希腊字母  
		csvFile_greeks << current_spot_1 << ","
			<< resultseuro.delta << "," << resultseuro.gamma << "," << resultsasia.delta << ","
			<< resultsasia.gamma << "\n";
	}

	csvFile_greeks.close();
	std::cout << "Full greeks generation completed! 'greeks_sensitivity.csv' is ready.\n\n";

	return 0;
}