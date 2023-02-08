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
#include <string>
#include <optional>

namespace ba {

	using MoneyType          = double;
	using MoneyDiffType      = double;
	using ShareType          = std::int32_t;
	using ID32               = std::uint32_t;
	using CommissionRateType = double;
	using ParamType          = double;

	struct OrderService
	{
		OrderType orderType{ OrderType::None };
		
		inline void ClosePosition() noexcept { orderType = OrderType::ClosePosition; }
		inline void OpenPosition()  noexcept { orderType = OrderType::OpenPosition; }
	};

	struct Bar
	{
		std::string date{ };
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
		const Bar          bar{ };
		const PositionType positionType{ PositionType::Closed };
		OrderService       orderService{ };
	};

	struct OrderLog
	{
		ID32      barNo{ 0 };
		MoneyType netWorth{ 0 };
		MoneyType balance{ 0 };
		MoneyType price{ 0 };
		ShareType positionAmount{ 0 };
		OrderType orderType{ OrderType::None };
	};

	class OrderLogger
	{
	public:
		std::vector<OrderLog> orderLogs;
		std::vector<MoneyType> barEndNetWorths;
		
		inline
		void add(const ID32 barNo, const MoneyType bid, const MoneyType balance, const MoneyType price, const ShareType positionAmount, const OrderType orderType) {
			
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
			
			barEndNetWorths.push_back(net_worth);
		}
	};

	struct TestSummary {
		const size_t                          totalOrders{ 0 };
		const MoneyType                       finalBalance{ 0 };
		const std::vector<ParamType>          params{ };
		std::optional<std::vector<OrderLog>>  orderLogs;
		std::optional<std::vector<MoneyType>> barEndNetWorths;
    };

}

#endif /* types_h */
