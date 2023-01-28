//
//  enums.h
//  BorsaAnaliz
//
//  Created by Samet Pilav on 24.04.2021.
//

#ifndef enums_h
#define enums_h

namespace ba {

	enum class PositionType
	{
		Closed, Opened
	};

	enum class OrderType
	{
		None, ClosePosition, OpenPosition
	};

	const char* to_string(PositionType positionType) {
		   switch (positionType) {
			   case PositionType::Closed:
				   return "Closed";
			   case PositionType::Opened:
				   return "Opened";
			   default:
				   return "None";
		   }
	   }

	const char* to_string(OrderType orderType) {
		   switch (orderType) {
			   case OrderType::ClosePosition:
				   return "Close";
			   case OrderType::OpenPosition:
				   return "Open";
			   default:
				   return "None";
		   }
	   }

}

#endif /* enums_h */
