/* Copyright(C) 2024 by Federico Sauter<federicosauter@pm.me>. All rights reserved. */

#include "munit/munit.h"

int main(int, char** argv)
{
    munit_assert_string_equal("test1", "test2");
}
