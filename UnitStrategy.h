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

	void OnStart(borsa::StartEvent& e) {
		
		e.orderService.Long();
	}

	void OnBarClosed(borsa::BarClosedEvent& e) { }

	void OnStop(borsa::StopEvent& e) {
		
		e.orderService.Close();
	}
};

#endif /* UnitStrategy_h */
