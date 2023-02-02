//
//  ticker.h
//  BorsaAnaliz
//
//  Created by Samet Pilav on 24.04.2021.
//

#ifndef ticker_h
#define ticker_h

#include "enums.h"
#include "types.h"
#include "utils.h"

namespace ba {

	class Ticker final
	{
	private:
		constexpr static double comissionRate = 0.0015;
		
		struct TickerState {
			unsigned int  barNo{ 0 };
			MoneyType         bid{ 0 };
			MoneyType         ask{ 0 };
			ShareType         positionAmount{ 0 };
			PositionType  positionType{ PositionType::Closed };
			MoneyType         balance{ 0 };
		};
		
	public:
		Ticker() = default;

		template <typename StrategyType>
		OrderLogger RunTest(StrategyType&& strategy, MoneyType balance, const std::vector<Bar>& bars) const noexcept {
			
			TickerState tickerState;
			tickerState.balance = balance;
			
			const MoneyType firstTick = CollectionUtils::getFirst(bars).value_or(Bar{}).open;
			const MoneyType lastTick = CollectionUtils::getLast(bars).value_or(Bar{}).close;
			
			OrderLogger orderLogger;
			
			this->Start(firstTick, strategy, tickerState, orderLogger);

			for (const Bar& bar : bars) {
				
				this->BarClosed(bar, strategy, tickerState, orderLogger);
				
				orderLogger.barClosed(tickerState.bid, tickerState.balance, tickerState.positionAmount);
				
				tickerState.barNo++;
			}

			this->Stop(lastTick, strategy, tickerState, orderLogger);
			
			return orderLogger;
		}
		
		template <typename StrategyType>
		std::vector<TestResult> RunMultiTest(const MoneyType balance, std::vector<double> iter1, std::vector<double> iter2) const noexcept {
			
			std::vector<TestResult> testResults;
			
			for (const double var1 : iter1) {
				for (const double var2 : iter2) {
					
					StrategyType strategy(var1, var2);
					
					const OrderLogger orderLogger = this->RunTest(strategy, balance);
					
					const OrderLog& lastLog = CollectionUtils::getLast(orderLogger.orderLogs).value_or(OrderLog{});
					
					TestResult testResult;
					testResult.netWorth = lastLog.netWorth;// lastLog.balance + lastLog.positionAmount * lastLog.price;
					testResult.orderLogger = std::move(orderLogger);
					testResult.var1 = var1;
					testResult.var2 = var2;
					
					testResults.push_back( std::move(testResult) );
				}
			}
			
			return testResults;
		}

	private:

		template <typename StrategyType>
		void Start(const MoneyType tick, StrategyType& strategy, TickerState& TickerState, OrderLogger& orderLogger) const noexcept {

			TickerState.bid = tick;
			TickerState.ask = tick + BarUtils::calculateStep(tick);

			StartEvent e = { TickerState.bid, TickerState.ask, TickerState.positionType };
			strategy.OnStart(e);

			this->ExecuteTheOrder(e.orderService, TickerState, orderLogger);
		}

		template <typename StrategyType>
		void Stop(const MoneyType tick, StrategyType& strategy, TickerState& TickerState, OrderLogger& orderLogger) const noexcept {

			TickerState.bid = tick;
			TickerState.ask = tick + BarUtils::calculateStep(tick);

			StopEvent e = { TickerState.bid, TickerState.ask, TickerState.positionType };
			strategy.OnStop(e);

			this->ExecuteTheOrder(e.orderService, TickerState, orderLogger);
		}

		template <typename StrategyType>
		void BarClosed(const Bar& bar, StrategyType& strategy, TickerState& TickerState, OrderLogger& orderLogger) const noexcept {

			const MoneyType tick = bar.close;
			TickerState.bid = tick;
			TickerState.ask = tick + BarUtils::calculateStep(tick);

			BarClosedEvent e = { TickerState.bid, TickerState.ask, bar, TickerState.positionType };
			strategy.OnBarClosed(e);
			
			this->ExecuteTheOrder(e.orderService, TickerState, orderLogger);
		}

		inline
		void ExecuteTheOrder(const OrderService& orderService, TickerState& TickerState, OrderLogger& orderLogger) const noexcept {
			
			const OrderType orderType = orderService.orderType;
			
			switch (orderType)
			{
				case OrderType::ClosePosition:
					this->TryClose(TickerState, orderLogger);
					break;
				case OrderType::OpenPosition:
					this->TryLong(TickerState, orderLogger);
					break;
				default:
					break;
			}
		}
		
		void TryClose(TickerState& TickerState, OrderLogger& orderLogger) const noexcept {
			
			if (PositionType::Opened == TickerState.positionType)
			{
				const MoneyType selling_price = TickerState.bid * (1 - comissionRate);
				
				TickerState.balance += TickerState.positionAmount * selling_price;
				TickerState.positionAmount = 0;
				TickerState.positionType = PositionType::Closed;
				
				orderLogger.add(TickerState.barNo, TickerState.bid, TickerState.balance, selling_price, TickerState.positionAmount, OrderType::ClosePosition);
			}
		}

		void TryLong(TickerState& TickerState, OrderLogger& orderLogger) const noexcept {
			
			if (PositionType::Closed == TickerState.positionType)
			{
				const MoneyType buying_price = TickerState.ask * (1 + comissionRate);
				
				TickerState.positionAmount = int(TickerState.balance / buying_price);
				TickerState.balance -= TickerState.positionAmount * buying_price;
				TickerState.positionType = PositionType::Opened;
				
				orderLogger.add(TickerState.barNo, TickerState.bid, TickerState.balance, buying_price, TickerState.positionAmount, OrderType::OpenPosition);
			}
		}
		
	};

}

#endif /* tahta_h */
