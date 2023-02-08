//
//  tester.h
//  BorsaAnaliz2
//
//  Created by Samet Pilav on 2.05.2021.
//

#ifndef tester_h
#define tester_h

#include <sstream>
#include <vector>
#include <map>
#include <fstream>
#include <string>

namespace ba {

	class Tester final
	{
	private:
		
		struct TestState
		{
			ID32               barNo{ 0 };
			MoneyType          bid{ 0 };
			MoneyType          ask{ 0 };
			ShareType          positionAmount{ 0 };
			PositionType       positionType{ PositionType::Closed };
			MoneyType          balance{ 0 };
			CommissionRateType commissionRate{ 0 };
		};
		
	public:
		
		template <typename StrategyType>
		static
		TestSummary RunTest(StrategyType&& strategy,
						    const std::vector<Bar>& bars,
						    const MoneyType balance,
						    const CommissionRateType commissionRate) noexcept
		{
			TestState testState;
			testState.balance = balance;
			testState.commissionRate = commissionRate / 100;
			
			const MoneyType firstTick = CollectionUtils::getFirst(bars).value_or(Bar{}).open;
			const MoneyType lastTick = CollectionUtils::getLast(bars).value_or(Bar{}).close;
			
			OrderLogger orderLogger;
			
			Start(firstTick, strategy, testState, orderLogger);
			
			for (const Bar& bar : bars) {
				
				BarClosed(bar, strategy, testState, orderLogger);
			}
			
			Stop(lastTick, strategy, testState, orderLogger);
			
			TestSummary summary = {
				.totalOrders     = orderLogger.orderLogs.size(),
				.finalBalance    = CollectionUtils::getLast(orderLogger.barEndNetWorths).value_or(MoneyType{0}),
				.params          = strategy.params(),
				.orderLogs       = orderLogger.orderLogs,
				.barEndNetWorths = orderLogger.barEndNetWorths
			};
			
			return summary;
		}
		
	private:
		
		template <typename StrategyType>
		static
		void Start(const MoneyType tick, StrategyType& strategy, TestState& testState, OrderLogger& orderLogger) noexcept
		{
			testState.bid = tick;
			testState.ask = tick + BarUtils::calculateStep(tick);
			
			StartEvent e = { testState.bid, testState.ask, testState.positionType };
			strategy.OnStart(e);
			
			ExecuteTheOrder(e.orderService, testState, orderLogger);
		}
		
		template <typename StrategyType>
		static
		void Stop(const MoneyType tick, StrategyType& strategy, TestState& testState, OrderLogger& orderLogger) noexcept
		{
			testState.bid = tick;
			testState.ask = tick + BarUtils::calculateStep(tick);
			
			StopEvent e = { testState.bid, testState.ask, testState.positionType };
			strategy.OnStop(e);
			
			ExecuteTheOrder(e.orderService, testState, orderLogger);
		}
		
		template <typename StrategyType>
		static
		void BarClosed(const Bar& bar, StrategyType& strategy, TestState& testState, OrderLogger& orderLogger) noexcept
		{
			const MoneyType tick = bar.close;
			testState.bid = tick;
			testState.ask = tick + BarUtils::calculateStep(tick);
			
			BarClosedEvent e = { testState.bid, testState.ask, bar, testState.positionType };
			strategy.OnBarClosed(e);
			
			ExecuteTheOrder(e.orderService, testState, orderLogger);
			
			orderLogger.barClosed(testState.bid, testState.balance, testState.positionAmount);
			
			testState.barNo++;
		}
		
		inline
		static
		void ExecuteTheOrder(const OrderService& orderService, TestState& testState, OrderLogger& orderLogger) noexcept
		{
			switch (orderService.orderType)
			{
				case OrderType::ClosePosition:
					TryClose(testState, orderLogger);
					break;
				case OrderType::OpenPosition:
					TryLong(testState, orderLogger);
					break;
				default:
					break;
			}
		}
		
		static
		void TryClose(TestState& testState, OrderLogger& orderLogger) noexcept
		{
			if (PositionType::Opened == testState.positionType)
			{
				const MoneyType selling_price = testState.bid * (1 - testState.commissionRate);
				
				testState.balance += testState.positionAmount * selling_price;
				testState.positionAmount = 0;
				testState.positionType = PositionType::Closed;
				
				orderLogger.add(testState.barNo, testState.bid, testState.balance, selling_price, testState.positionAmount, OrderType::ClosePosition);
			}
		}
		
		static
		void TryLong(TestState& testState, OrderLogger& orderLogger) noexcept
		{
			if (PositionType::Closed == testState.positionType) {
				
				const MoneyType buying_price = testState.ask * (1 + testState.commissionRate);
				
				testState.positionAmount = int(testState.balance / buying_price);
				testState.balance -= testState.positionAmount * buying_price;
				testState.positionType = PositionType::Opened;
				
				orderLogger.add(testState.barNo, testState.bid, testState.balance, buying_price, testState.positionAmount, OrderType::OpenPosition);
			}
		}

	public:
		
		template<typename StrategyType>
		static
		auto RunTestOnManyStocksForGeneralOptimization(
			const std::map<std::string, std::vector<Bar>>& tickerNameToBarsMap,
			const std::vector<ParamType>& paramsForRow,
			const std::vector<ParamType>& paramsForColumn,
			const MoneyType balance,
			const CommissionRateType commissionRate,
			const std::string& outputFileName)
		{
			
			constexpr char TAB = ';';
			constexpr char ENDL = '\n';
			
			std::ostringstream report;
			report.imbue(std::locale("de_DE"));
			
			report << TAB;
			for (auto param_for_row : paramsForRow) {
				report << param_for_row << TAB;
			}
			report << ENDL;
			
			for (auto param_for_row : paramsForRow) {
				report << param_for_row << TAB;
				
				for (auto param_for_column : paramsForColumn) {
					
					MoneyType sum_of_total_balances = 0;
					
					for (const auto& [ticker_name, bars] : tickerNameToBarsMap) {
						
						StrategyType strategy(param_for_row, param_for_column);
						
						const TestSummary summary = Tester::RunTest(strategy, bars, balance, commissionRate);
						sum_of_total_balances += summary.finalBalance;
						
					}
					
					const double gain = sum_of_total_balances / (balance * tickerNameToBarsMap.size());
					report << gain << TAB;
				}
				
				report << ENDL;
			}
			
			if (outputFileName != "") {
				std::ofstream of(outputFileName);
				of << report.str();
			}
		}
		
		template<typename StrategyType>
		static
		auto RunTestUsingParamPermutations(
			const std::vector<std::vector<ParamType>>& paramPermutations,
			const std::vector<Bar>& bars,
			const MoneyType balance,
			const CommissionRateType commissionRate) noexcept
		{
			std::vector<TestSummary> summaries;
			summaries.reserve(paramPermutations.size());
			
			for (const auto& params : paramPermutations) {
				
				StrategyType strategy {params};
				
				TestSummary summary = Tester::RunTest(strategy, bars, balance, commissionRate);
				
				summaries.emplace_back(std::move(summary));
			}
			return summaries;
		}
		
	};

}


#endif /* tester_h */
