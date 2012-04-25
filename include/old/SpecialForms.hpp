#ifndef __SPECIAL_FORMS_HPP__
#define __SPECIAL_FORMS_HPP__ 1

#include "Value.hpp"

namespace lllm {
	bool isSpecialForm( ConsPtr form );

	bool isQuoteForm( ConsPtr form );
	bool isIfForm( ConsPtr form );
	bool isDoForm( ConsPtr form );
	bool isDefineForm( ConsPtr form );
	bool isLetForm( ConsPtr form );
	bool isLambdaForm( ConsPtr form );
	bool isThunkForm( ConsPtr form );
}

#endif /* __SPECIAL_FORMS_HPP__ */

