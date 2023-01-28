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
	//borsa::Money longed_at{ 0 };
	borsa::Money upper_sell_level{ 0 };
	borsa::Money lower_sell_level{ 0 };

public:
	
	OcoStrategy(double upper_sell_percentage, double lower_sell_percentage)
	: upper_sell_percentage(1 + upper_sell_percentage / 100)
	, lower_sell_percentage(1 - lower_sell_percentage / 100)
	{}

	void OnStart(borsa::StartEvent& e) {
		
		upper_sell_level = e.bid * upper_sell_percentage;
		lower_sell_level = e.bid * lower_sell_percentage;
		e.orderService.Long();
	}

	void OnBarClosed(borsa::BarClosedEvent& e) {
		
		const borsa::PositionType positionType = e.positionType;
		
		switch (positionType) {
			case borsa::PositionType::Closed:
				
				if ((rand() & 0x1) == 0) {

					upper_sell_level = e.bid * upper_sell_percentage;
					lower_sell_level = e.bid * lower_sell_percentage;
					e.orderService.Long();
				}
				
				break;
			case borsa::PositionType::Longed:
				
				if (e.bid < lower_sell_level || upper_sell_level < e.bid) {
					
					e.orderService.Close();
				}
				
				break;
			case borsa::PositionType::Shorted:
				break;
		}
	}

	void OnStop(borsa::StopEvent& e) {
		
		e.orderService.Close();
	}
};

#endif /* OcoStrategy_h */
