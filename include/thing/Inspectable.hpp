#ifndef INSPECTABLE_HPP
#define INSPECTABLE_HPP

#include <memory>
#include <string>

class Thing;

class Inspectable
{
  public:
	Inspectable();

	virtual const std::string onInspect(const std::shared_ptr<Thing>& owner,
										const std::shared_ptr<Thing>& inspector);
	virtual const std::string onHelp(std::shared_ptr<Thing> owner,
									 std::shared_ptr<Thing> inspector);
	virtual const std::string getName(const std::shared_ptr<Thing>& owner);
};

#endif
