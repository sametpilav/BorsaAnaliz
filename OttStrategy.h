//
//  OttStrategy.h
//  BorsaAnaliz
//
//  Created by Samet Pilav on 14.05.2021.
//

#ifndef OttStrategy_h
#define OttStrategy_h

#include "borsa/borsa.h"
#include <algorithm>

class OttStrategy final {
	
private:
	const ba::ParamType stoploss_percentage_to_buy;
	const ba::ParamType stoploss_percentage_to_sell;
	ba::MoneyType furthest_bid{};
	ba::MoneyType stoploss_value{};
	
public:
	
	OttStrategy(const ba::ParamType stoploss_percentage_to_buy, const ba::ParamType stoploss_percentage_to_sell) noexcept
	: stoploss_percentage_to_buy(1 + stoploss_percentage_to_buy / 100)
	, stoploss_percentage_to_sell(1 - stoploss_percentage_to_sell / 100)
	{}
	
	OttStrategy(const std::vector<ba::ParamType>& params)
	: stoploss_percentage_to_buy(1 + params.at(0) / 100)
	, stoploss_percentage_to_sell(1 - params.at(1) / 100)
	{}
	
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
				this->ApplyRulesForLongedPosition(e);
				break;
		}
	}

	void OnStop(ba::StopEvent& e) noexcept {
		
		e.orderService.ClosePosition();
	}
	
private:
	
	void AppyRulesForClosedPosition(ba::BarClosedEvent& e) noexcept {
		
		ba::OrderService& orderService = e.orderService;
		
		// yön: aşağı
		if (e.bid < this->furthest_bid) {
			this->furthest_bid = e.bid;
			this->stoploss_value = std::min(this->stoploss_value, static_cast<ba::MoneyType>(e.bid * this->stoploss_percentage_to_buy));
		}
		
		// yön: yukarı
		if (e.bid > this->stoploss_value) {
			this->furthest_bid = e.bid;
			orderService.OpenPosition();
		}
	}
	
	void ApplyRulesForLongedPosition(ba::BarClosedEvent& e) noexcept {
		
		ba::OrderService& orderService = e.orderService;
		
		// yön: yukarı
		if (e.bid > this->furthest_bid) {
			this->furthest_bid = e.bid;
			this->stoploss_value = std::max(this->stoploss_value, static_cast<ba::MoneyType>(e.bid * this->stoploss_percentage_to_sell));
		}
		
		// yön: aşağı
		if (e.bid < this->stoploss_value) {
			this->furthest_bid = e.bid;
			orderService.ClosePosition();
		}
	}
};

#endif /* OttStrategy_h */
