//
//  UnitStrategy.h
//  BorsaAnaliz
//
//  Created by Samet Pilav on 4.05.2021.
//

#ifndef UnitStrategy_h
#define UnitStrategy_h


#include "borsa/borsa.h"

class UnitStrategy final {
	
public:

	UnitStrategy(double = 0, double = 0) { }

	void OnStart(ba::StartEvent& e) {
		
		e.orderService.OpenPosition();
	}

	void OnBarClosed(ba::BarClosedEvent& e) { }

	void OnStop(ba::StopEvent& e) {
		
		e.orderService.ClosePosition();
	}
};

#endif /* UnitStrategy_h */
