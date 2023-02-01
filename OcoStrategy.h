//
//  OcoStrategy.h
//  BorsaAnaliz
//
//  Created by Samet Pilav on 15.05.2021.
//

#ifndef OcoStrategy_h
#define OcoStrategy_h


#include "borsa/borsa.h"

class OcoStrategy final {
	const double upper_sell_percentage;
	const double lower_sell_percentage;
	//ba::Money longed_at{ 0 };
	ba::MoneyType upper_sell_level{ 0 };
	ba::MoneyType lower_sell_level{ 0 };

public:
	
	OcoStrategy(double upper_sell_percentage, double lower_sell_percentage)
	: upper_sell_percentage(1 + upper_sell_percentage / 100)
	, lower_sell_percentage(1 - lower_sell_percentage / 100)
	{}

	void OnStart(ba::StartEvent& e) {
		
		upper_sell_level = e.bid * upper_sell_percentage;
		lower_sell_level = e.bid * lower_sell_percentage;
		e.orderService.OpenPosition();
	}

	void OnBarClosed(ba::BarClosedEvent& e) {
		
		const ba::PositionType positionType = e.positionType;
		
		switch (positionType) {
			case ba::PositionType::Closed:
				
				if ((rand() & 0x1) == 0) {

					upper_sell_level = e.bid * upper_sell_percentage;
					lower_sell_level = e.bid * lower_sell_percentage;
					e.orderService.OpenPosition();
				}
				
				break;
			case ba::PositionType::Opened:
				
				if (e.bid < lower_sell_level || upper_sell_level < e.bid) {
					
					e.orderService.ClosePosition();
				}
				
				break;
		}
	}

	void OnStop(ba::StopEvent& e) {
		
		e.orderService.ClosePosition();
	}
};

#endif /* OcoStrategy_h */
