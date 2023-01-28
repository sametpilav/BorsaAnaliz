//
//  types.h
//  BorsaAnaliz
//
//  Created by Samet Pilav on 24.04.2021.
//

#ifndef types_h
#define types_h

#include "enums.h"

#include <cstdint>
#include <vector>

namespace ba {

	using Money = float;
	using MoneyDiff = float;

	struct OrderService
	{
		OrderType orderType{ OrderType::None };
		
		inline void ClosePosition() noexcept { orderType = OrderType::ClosePosition; }
		inline void OpenPosition()  noexcept { orderType = OrderType::OpenPosition; }
	};

	struct Bar
	{
		std::string date;
		Money open{ 0 };
		Money high{ 0 };
		Money low{ 0 };
		Money close{ 0 };
	};

	struct StartEvent
	{
		const Money        bid{ 0 };
		const Money        ask{ 0 };
		const PositionType positionType{ PositionType::Closed };
		OrderService       orderService{ };
	};

	struct StopEvent
	{
		const Money        bid{ 0 };
		const Money        ask{ 0 };
		const PositionType positionType{ PositionType::Closed };
		OrderService       orderService{ };
	};

	struct BarClosedEvent
	{
		const Money        bid{ 0 };
		const Money        ask{ 0 };
		const Bar          bar{ 0, 0, 0, 0 };
		const PositionType positionType{ PositionType::Closed };
		OrderService       orderService{ };
	};

	struct OrderRecord {
		OrderType     orderType{ OrderType::None };
		Money         unit_price{ 0 };
		Money         net_worth { 0 };
		std::uint32_t amount{ 0 };
		std::uint32_t event_id{ 0 };
	};

	struct OrderLog {
		std::uint32_t barNo{ 0 };
		Money         netWorth{ 0 };
		Money         balance{ 0 };
		Money         price{ 0 };
		std::uint32_t positionAmount{ 0 };
		OrderType     orderType{ OrderType::None };
	};

	class OrderLogger {
		
	public:
		std::vector<OrderLog> orderLogs;
		std::vector<Money> bar_end_net_worths;
		
		inline
		void add(const std::uint32_t barNo, const Money bid, const Money balance, const Money price, const std::uint32_t positionAmount, const OrderType orderType) {
			
			const Money net_worth = balance + bid * positionAmount;
			
			OrderLog orderLog;
			orderLog.barNo = barNo;
			orderLog.netWorth = net_worth;
			orderLog.balance = balance;
			orderLog.price = price;
			orderLog.positionAmount = positionAmount;
			orderLog.orderType = orderType;
			
			orderLogs.push_back( orderLog );
		}
		
		inline
		void barClosed(const Money bid, const Money balance, const std::uint32_t positionAmount) {
			
			const Money net_worth = balance + bid * positionAmount;
			
			bar_end_net_worths.push_back(net_worth);
		}
	};

	struct TestResult {
		OrderLogger orderLogger{ };
		Money       netWorth{ 0 };
		double      var1{ 0 };
		double      var2{ 0 };
	};

	struct TestResultSameParameterMultiStock {
		double                    stoploss_percentage_to_buy;
		double                    stoploss_percentage_to_sell;
		std::vector<ba::Money> gain_history;
	};

}

#endif /* types_h */
