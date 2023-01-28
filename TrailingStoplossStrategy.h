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
	const double stoploss_percentage_to_buy;
	const double stoploss_percentage_to_sell;
	ba::Money furthest_bid{};
	ba::Money stoploss_value{};
	
public:

	TrailingStoplossStrategy(const double stoploss_percentage_to_buy, const double stoploss_percentage_to_sell)
		: stoploss_percentage_to_buy(1 + stoploss_percentage_to_buy / 100)
		, stoploss_percentage_to_sell(1 - stoploss_percentage_to_sell / 100)
	{}

	void OnStart(ba::StartEvent& e) {
		
		this->furthest_bid = e.bid;
		this->stoploss_value = e.bid * this->stoploss_percentage_to_buy;
	}

	void OnBarClosed(ba::BarClosedEvent& e) {
		
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

	void OnStop(ba::StopEvent& e) {

		//e.orderService.Close();
	}
	
private:
	
	void AppyRulesForClosedPosition(ba::BarClosedEvent& e) {
		
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
	
	void ApplyRulesForLongedPosition(ba::BarClosedEvent& e) {
		
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