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

	UnitStrategy(ba::ParamType = 0, ba::ParamType = 0) { }
	
	UnitStrategy(const std::vector<ba::ParamType>&) { }
	
	std::vector<ba::ParamType> params() const noexcept {
		
		return {};
	}

	void OnStart(ba::StartEvent& e) {
		
		e.orderService.OpenPosition();
	}

	void OnBarClosed(ba::BarClosedEvent& e) { }

	void OnStop(ba::StopEvent& e) {
		
		e.orderService.ClosePosition();
	}
};

#endif /* UnitStrategy_h */
