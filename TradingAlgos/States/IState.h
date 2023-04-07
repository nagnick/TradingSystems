#pragma once
#include "Streams/StreamData.h"

#include <memory>

class IState{
    // dragged over from the IDataStreamSubscriber duplicate code but want to avoid dragging anything unnecessary from the IDataStreamSub as it grows out
    // only want the onData methods
    virtual void onData(std::shared_ptr<IStreamData> data) = 0; // defualt case
    virtual void onData(std::shared_ptr<OtherData> other) = 0;
    virtual void onData(std::shared_ptr<TradeData> trade) = 0;
    virtual void onData(std::shared_ptr<QuoteData> quote) = 0;
};