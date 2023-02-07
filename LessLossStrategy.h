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
	const ba::ParamType stoploss_percentage_to_buy;
	const ba::ParamType stoploss_percentage_to_sell;
	ba::MoneyType furthest_bid{};
	ba::MoneyType stoploss_value{};
	ba::MoneyType proposal_stoploss_value{};
	
public:
	
	LessLossStrategy(const ba::ParamType stoploss_percentage_to_buy, const ba::ParamType stoploss_percentage_to_sell)
	: stoploss_percentage_to_buy(1 + stoploss_percentage_to_buy / 100)
	, stoploss_percentage_to_sell(1 - stoploss_percentage_to_sell / 100)
	{}
	
	LessLossStrategy(const std::vector<ba::ParamType>& params)
	: stoploss_percentage_to_buy(1 + params.at(0) / 100)
	, stoploss_percentage_to_sell(1 - params.at(1) / 100)
	{}
	
	std::vector<ba::ParamType> params() const noexcept {
		return std::vector{stoploss_percentage_to_buy * 100 - 100, 100 - stoploss_percentage_to_sell * 100};
	}

	void OnStart(ba::StartEvent& e) {
		
//		ba::OrderService& orderService = e.orderService;
//		orderService.Long();
//
//		this->furthest_bid = e.bid;
//		this->stoploss_value = e.bid * this->stoploss_percentage_to_sell;
		
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
		//std::cout << "\t" << this->stoploss_value << "\n";
	}

	void OnStop(ba::StopEvent& e) {
		
		e.orderService.ClosePosition();
	}
	
private:
	
	void AppyRulesForClosedPosition(ba::BarClosedEvent& e) {
		
		ba::OrderService& orderService = e.orderService;
		
		// fiyat aşağı düşmeye devam ederse
		if (e.bid < this->furthest_bid) {
			this->furthest_bid = e.bid;
			this->stoploss_value = std::min(this->stoploss_value, static_cast<ba::MoneyType>(e.bid * this->stoploss_percentage_to_buy));
			this->proposal_stoploss_value = this->stoploss_value;
		}
		
		// fiyat, tetiği kırarsa
		if (e.bid > this->stoploss_value) {
			this->furthest_bid = e.bid;
			//this->stoploss_value = e.bid * this->stoploss_percentage_to_sell;
			orderService.OpenPosition();
		}
	}
	
	void ApplyRulesForLongedPosition(ba::BarClosedEvent& e) {
		
		ba::OrderService& orderService = e.orderService;
		
		// yön: yukarı
		if (e.bid > this->furthest_bid) {
			this->furthest_bid = e.bid;
			this->stoploss_value = std::max(this->stoploss_value, static_cast<ba::MoneyType>(e.bid * this->stoploss_percentage_to_sell));
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
			orderService.ClosePosition();
		}
	}
};

#endif /* LessLossStrategy_h */
