//
//  LessLossStrategy.h
//  BorsaAnaliz
//
//  Created by Samet Pilav on 8.05.2021.
//

#ifndef LessLossStrategy_h
#define LessLossStrategy_h

#include "borsa/borsa.h"
#include <algorithm>

class LessLossStrategy final {
	
private:
	const double stoploss_percentage_to_buy;
	const double stoploss_percentage_to_sell;
	borsa::Money furthest_bid{};
	borsa::Money stoploss_value{};
	borsa::Money proposal_stoploss_value{};
	
public:
	
	LessLossStrategy(const double stoploss_percentage_to_buy,
					 const double stoploss_percentage_to_sell)
	: stoploss_percentage_to_buy(1 + stoploss_percentage_to_buy / 100)
	, stoploss_percentage_to_sell(1 - stoploss_percentage_to_sell / 100)
	{}

	void OnStart(borsa::StartEvent& e) {
		
//		borsa::OrderService& orderService = e.orderService;
//		orderService.Long();
//
//		this->furthest_bid = e.bid;
//		this->stoploss_value = e.bid * this->stoploss_percentage_to_sell;
		
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
		//std::cout << "\t" << this->stoploss_value << "\n";
	}

	void OnStop(borsa::StopEvent& e) {
		
		e.orderService.Close();
	}
	
private:
	
	void AppyRulesForClosedPosition(borsa::BarClosedEvent& e) {
		
		borsa::OrderService& orderService = e.orderService;
		
		// fiyat aşağı düşmeye devam ederse
		if (e.bid < this->furthest_bid) {
			this->furthest_bid = e.bid;
			this->stoploss_value = std::min(this->stoploss_value, static_cast<borsa::Money>(e.bid * this->stoploss_percentage_to_buy));
			this->proposal_stoploss_value = this->stoploss_value;
		}
		
		// fiyat, tetiği kırarsa
		if (e.bid > this->stoploss_value) {
			this->furthest_bid = e.bid;
			//this->stoploss_value = e.bid * this->stoploss_percentage_to_sell;
			orderService.Long();
		}
	}
	
	void ApplyRulesForLongedPosition(borsa::BarClosedEvent& e) {
		
		borsa::OrderService& orderService = e.orderService;
		
		// yön: yukarı
		if (e.bid > this->furthest_bid) {
			this->furthest_bid = e.bid;
			this->stoploss_value = std::max(this->stoploss_value, static_cast<borsa::Money>(e.bid * this->stoploss_percentage_to_sell));
			this->proposal_stoploss_value = std::max(this->proposal_stoploss_value, this->stoploss_value);
		}
		// yön: yatay, fiyat furthest_bid ve stoploss_value arasında
		else if (e.bid > this->stoploss_value) { // e.bid > this->furthest_bid
			this->proposal_stoploss_value = std::max(this->proposal_stoploss_value, (this->furthest_bid + 3*this->stoploss_value) / 4);
		}
		
		// yön: aşağı
		if (e.bid < this->stoploss_value || e.bid < this->proposal_stoploss_value) {
			this->furthest_bid = e.bid;
			this->stoploss_value = this->proposal_stoploss_value;
			orderService.Close();
		}
	}
};

#endif /* LessLossStrategy_h */
