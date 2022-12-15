#!/usr/bin/env python

import sys
import clang.cindex



index = clang.cindex.Index.create()
tu = index.parse("Test.cpp")
tu.save("Test.generated.cpp")
print('Translation unit: ', tu.spelling)
for child in tu.cursor.get_children():
    if (child.kind.is_attribute()):
        print(child.spelling)