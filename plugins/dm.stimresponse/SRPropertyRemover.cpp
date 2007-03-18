#include "SRPropertyRemover.h"

#include "iregistry.h"
#include "entitylib.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/regex.hpp>

SRPropertyRemover::SRPropertyRemover(
	Entity* target, 
	SREntity::KeyList& keys
) :
	_target(target),
	_keys(keys)
{}

SRPropertyRemover::~SRPropertyRemover() {
	for (unsigned int i = 0; i < _removeList.size(); i++) {
		// Delete the key
		_target->setKeyValue(_removeList[i], "");
	}
}

void SRPropertyRemover::visit(const std::string& key, const std::string& value) {
	std::string prefix = GlobalRegistry().get(RKEY_STIM_RESPONSE_PREFIX);
	
	// Now cycle through the possible key names and see if we have a match
	for (unsigned int i = 0; i < _keys.size(); i++) {
		
		// Construct a regex with the number as match variable
		std::string exprStr = "^" + prefix + _keys[i].key + "_([0-9])+$";
		boost::regex expr(exprStr);
		boost::smatch matches;
		
		if (boost::regex_match(key, matches, expr)) {
			// We have a match, set the key on the black list
			_removeList.push_back(key);
		}
	}
	
	// Check the key/value for script definitions
	StimTypeMap& stimMap = _stimTypes.getStimMap();
	for (StimTypeMap::iterator i = stimMap.begin(); i != stimMap.end(); i++) {
		// Construct a regex with the number as match variable
		std::string exprStr = prefix + "script_" + i->second.name;
		
		if (key == exprStr) {
			// We have a match, set the key on the black list
			_removeList.push_back(key);
		}
	}
}
