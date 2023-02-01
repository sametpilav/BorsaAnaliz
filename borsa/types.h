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

	using MoneyType = float;
	using MoneyDiffType = float;
	using ShareType = std::int32_t;

	struct OrderService
	{
		OrderType orderType{ OrderType::None };
		
		inline void ClosePosition() noexcept { orderType = OrderType::ClosePosition; }
		inline void OpenPosition()  noexcept { orderType = OrderType::OpenPosition; }
	};

	struct Bar
	{
		std::string date;
		MoneyType   open{ 0 };
		MoneyType   high{ 0 };
		MoneyType   low{ 0 };
		MoneyType   close{ 0 };
	};

	struct StartEvent
	{
		const MoneyType    bid{ 0 };
		const MoneyType    ask{ 0 };
		const PositionType positionType{ PositionType::Closed };
		OrderService       orderService{ };
	};

	struct StopEvent
	{
		const MoneyType    bid{ 0 };
		const MoneyType    ask{ 0 };
		const PositionType positionType{ PositionType::Closed };
		OrderService       orderService{ };
	};

	struct BarClosedEvent
	{
		const MoneyType    bid{ 0 };
		const MoneyType    ask{ 0 };
		const Bar          bar{ 0, 0, 0, 0 };
		const PositionType positionType{ PositionType::Closed };
		OrderService       orderService{ };
	};

	struct OrderRecord {
		OrderType     orderType{ OrderType::None };
		MoneyType     unit_price{ 0 };
		MoneyType     net_worth { 0 };
		ShareType     amount{ 0 };
		std::uint32_t event_id{ 0 };
	};

	struct OrderLog {
		std::uint32_t barNo{ 0 };
		MoneyType     netWorth{ 0 };
		MoneyType     balance{ 0 };
		MoneyType     price{ 0 };
		ShareType     positionAmount{ 0 };
		OrderType     orderType{ OrderType::None };
	};

	class OrderLogger {
		
	public:
		std::vector<OrderLog> orderLogs;
		std::vector<MoneyType> bar_end_net_worths;
		
		inline
		void add(const std::uint32_t barNo, const MoneyType bid, const MoneyType balance, const MoneyType price, const ShareType positionAmount, const OrderType orderType) {
			
			const MoneyType net_worth = balance + bid * positionAmount;
			
			orderLogs.emplace_back(OrderLog {
				.barNo = barNo,
				.netWorth = net_worth,
				.balance = balance,
				.price = price,
				.positionAmount = positionAmount,
				.orderType = orderType
			});
		}
		
		inline
		void barClosed(const MoneyType bid, const MoneyType balance, const ShareType positionAmount) {
			
			const MoneyType net_worth = balance + bid * positionAmount;
			
			bar_end_net_worths.push_back(net_worth);
		}
	};

	struct TestResult {
		OrderLogger orderLogger{ };
		MoneyType   netWorth{ 0 };
		double      var1{ 0 };
		double      var2{ 0 };
	};

	struct TestResultSameParameterMultiStock {
		double                 stoploss_percentage_to_buy;
		double                 stoploss_percentage_to_sell;
		std::vector<MoneyType> gain_history;
	};

}

#endif /* types_h */
