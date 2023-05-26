//
//  TrailingStoplossStrategy.h
//  BorsaAnaliz
//
//  Created by Samet Pilav on 2.05.2021.
//

#ifndef TrailingStoplossStrategy_h
#define TrailingStoplossStrategy_h

#include "borsa/borsa.h"

class TrailingStoplossStrategy final {
	
private:
	const ba::ParamType stoploss_percentage_to_buy;
	const ba::ParamType stoploss_percentage_to_sell;
	ba::MoneyType furthest_bid{};
	ba::MoneyType stoploss_value{};
	
public:

	TrailingStoplossStrategy(const ba::ParamType stoploss_percentage_to_buy, const ba::ParamType stoploss_percentage_to_sell)
		: stoploss_percentage_to_buy(1 + stoploss_percentage_to_buy / 100)
		, stoploss_percentage_to_sell(1 - stoploss_percentage_to_sell / 100)
	{ }
	
	TrailingStoplossStrategy(const std::vector<ba::ParamType>& params)
	: stoploss_percentage_to_buy(1 + params.at(0) / 100)
	, stoploss_percentage_to_sell(1 - params.at(1) / 100)
	{ }
	
	std::vector<ba::ParamType> params() const noexcept {
		return std::vector{stoploss_percentage_to_buy * 100 - 100, 100 - stoploss_percentage_to_sell * 100};
	}

	void OnStart(ba::StartEvent& e) noexcept {
		
		this->furthest_bid = e.bid;
		this->stoploss_value = e.bid * this->stoploss_percentage_to_buy;
	}

	void OnBarClosed(ba::BarClosedEvent& e) noexcept {
		
		const ba::PositionType positionType = e.positionType;

		switch (positionType) {
			case ba::PositionType::Closed:
				this->AppyRulesForClosedPosition(e);
				break;
			case ba::PositionType::Opened:
				this->ApplyRulesForOpenedPosition(e);
				break;
		}
	}

	void OnStop(ba::StopEvent& e) noexcept {
	}
	
private:
	
	void AppyRulesForClosedPosition(ba::BarClosedEvent& e) noexcept {
		
		ba::OrderService& orderService = e.orderService;
		
		if (e.bid < this->furthest_bid) {
			this->furthest_bid = e.bid;
			this->stoploss_value = e.bid * this->stoploss_percentage_to_buy;
		}
		
		if (e.bid > this->stoploss_value) {
			this->furthest_bid = e.bid;
			this->stoploss_value = e.bid * this->stoploss_percentage_to_sell;
			orderService.OpenPosition();
		}
	}
	
	void ApplyRulesForOpenedPosition(ba::BarClosedEvent& e) noexcept {
		
		ba::OrderService& orderService = e.orderService;
		
		if (e.bid > this->furthest_bid) {
			this->furthest_bid = e.bid;
			this->stoploss_value = e.bid * this->stoploss_percentage_to_sell;
		}
		
		if (e.bid < this->stoploss_value) {
			this->furthest_bid = e.bid;
			this->stoploss_value = e.bid * this->stoploss_percentage_to_buy;
			orderService.ClosePosition();
		}
	}
};

#endif /* TrailingStoplossStrategy_h */
