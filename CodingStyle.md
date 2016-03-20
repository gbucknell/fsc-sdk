# Introduction #

The definition of a coding convention is a huge work that has been very well covered in the book Industrial Strength C++. This free ebook is available from [PASSAGEN](http://hem.passagen.se/erinyq/industrial/) and is a must-read for beginnners.

Mats Henricson and Erik Nyquist once said :

> "Code is always written in a particular style. Naming conventions, file name extensions and lexical style are all part of this structure of code we call Style. Discussing style is highly controversial, which is the reason we have placed it in an appendix, to keep it distinct from all other rules and recommendations."

# Simple is beautiful #
Developppers should use **the coding style of the C++ Standard Library** which can be sum up as follow :
  * All names (types, class, variables except as noted below) should be all lowercase, with words separated by underscores.
  * Acronyms should be treated as ordinary names (e.g. `xml_parser` instead of `XML_parser`).
  * Function parameters are prefixed with an underscore (e.g. `void compress(const int& _ratio)`
  * Member variable are postfixed with an underscore (e.g. `int ratio_;`)
  * Template parameter names should be PascalCase and ends with an uppercase T (e.g. `template <class ValueT>`).
  * Macro names are all uppercase and begin with FSC