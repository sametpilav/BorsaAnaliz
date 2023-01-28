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
	const double stoploss_percentage_to_buy;
	const double stoploss_percentage_to_sell;
	borsa::Money furthest_bid{};
	borsa::Money stoploss_value{};
	
public:
	
	OttStrategy(const double stoploss_percentage_to_buy,
					 const double stoploss_percentage_to_sell)
	: stoploss_percentage_to_buy(1 + stoploss_percentage_to_buy / 100)
	, stoploss_percentage_to_sell(1 - stoploss_percentage_to_sell / 100)
	{}

	void OnStart(borsa::StartEvent& e) {
		
		this->furthest_bid = e.bid;
		this->stoploss_value = e.bid * this->stoploss_percentage_to_buy;
	}

	void OnBarClosed(borsa::BarClosedEvent& e) {
		
		const borsa::PositionType positionType = e.positionType;

		switch (positionType) {
			case borsa::PositionType::Closed:
				this->AppyRulesForClosedPosition(e);
				break;
			case borsa::PositionType::Longed:
				this->ApplyRulesForLongedPosition(e);
				break;
			case borsa::PositionType::Shorted:
				break;
		}
	}

	void OnStop(borsa::StopEvent& e) {
		
		e.orderService.Close();
	}
	
private:
	
	void AppyRulesForClosedPosition(borsa::BarClosedEvent& e) {
		
		borsa::OrderService& orderService = e.orderService;
		
		// yön: aşağı
		if (e.bid < this->furthest_bid) {
			this->furthest_bid = e.bid;
			this->stoploss_value = std::min(this->stoploss_value, static_cast<borsa::Money>(e.bid * this->stoploss_percentage_to_buy));
		}
		
		// yön: yukarı
		if (e.bid > this->stoploss_value) {
			this->furthest_bid = e.bid;
			orderService.Long();
		}
	}
	
	void ApplyRulesForLongedPosition(borsa::BarClosedEvent& e) {
		
		borsa::OrderService& orderService = e.orderService;
		
		// yön: yukarı
		if (e.bid > this->furthest_bid) {
			this->furthest_bid = e.bid;
			this->stoploss_value = std::max(this->stoploss_value, static_cast<borsa::Money>(e.bid * this->stoploss_percentage_to_sell));
		}
		
		// yön: aşağı
		if (e.bid < this->stoploss_value) {
			this->furthest_bid = e.bid;
			orderService.Close();
		}
	}
};

#endif /* OttStrategy_h */
