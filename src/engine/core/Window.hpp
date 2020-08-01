#pragma once

#include "engine/core/base.hpp"
#include "engine/core/events/Event.h"

namespace opfor {

struct WindowProps
{
	std::string title;
	unsigned int width;
	unsigned int height;
};

class IWindow
{
private:

public:
	using EventCallbackFn = std::function<void(Event&)>;

public:
	virtual ~IWindow() {}

	virtual void HideCursor(bool) const = 0;
	virtual void *GetRawHandle() const = 0;
	virtual void Close() const = 0;

	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;

	virtual bool IsClosed() const = 0;

	virtual void SetEventCallback(const EventCallbackFn &) = 0;

	static UniquePtr<IWindow> Create(const WindowProps &props);
};

}
