# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_code_coverage.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CODE_COVERAGE()
#
# DESCRIPTION
#
#   Defines CODE_COVERAGE_CPPFLAGS, CODE_COVERAGE_CFLAGS,
#   CODE_COVERAGE_CXXFLAGS and CODE_COVERAGE_LIBS which should be included
#   in the CPPFLAGS, CFLAGS CXXFLAGS and LIBS/LIBADD variables of every
#   build target (program or library) which should be built with code
#   coverage support. Also defines CODE_COVERAGE_RULES which should be
#   substituted in your Makefile; and $enable_code_coverage which can be
#   used in subsequent configure output. CODE_COVERAGE_ENABLED is defined
#   and substituted, and corresponds to the value of the
#   --enable-code-coverage option, which defaults to being disabled.
#
#   Test also for coverage program and create COV_TOOL variable that could be
#   substituted.
#
#   Note that all optimization flags in CFLAGS must be disabled when code
#   coverage is enabled.
#
#   Usage example:
#
#   configure.ac:
#
#     AX_CODE_COVERAGE
#
#   Makefile.am:
#
#     @CODE_COVERAGE_RULES@
#     my_program_LIBS = ... $(CODE_COVERAGE_LIBS) ...
#     my_program_CPPFLAGS = ... $(CODE_COVERAGE_CPPFLAGS) ...
#     my_program_CFLAGS = ... $(CODE_COVERAGE_CFLAGS) ...
#     my_program_CXXFLAGS = ... $(CODE_COVERAGE_CXXFLAGS) ...
#
#   This results in a "check-code-coverage" rule being added to any
#   Makefile.am which includes "@CODE_COVERAGE_RULES@" (assuming the module
#   has been configured with --enable-code-coverage). Running `make
#   check-code-coverage` in that directory will run the module's test suite
#   (`make check`) and build a code coverage report detailing the code which
#   was touched, then print the URI for the report.
#
#   In earlier versions of this macro, CODE_COVERAGE_LDFLAGS was defined
#   instead of CODE_COVERAGE_LIBS. They are both still defined, but use of
#   CODE_COVERAGE_LIBS is preferred for clarity; CODE_COVERAGE_LDFLAGS is
#   deprecated. They have the same value.
#
#   This code was derived from Makefile.decl in GLib, originally licenced
#   under LGPLv2.1+.
#
# LICENSE
#
#   Copyright (c) 2012, 2016 Philip Withnall
#   Copyright (c) 2012 Xan Lopez
#   Copyright (c) 2012 Christian Persch
#   Copyright (c) 2012 Paolo Borelli
#   Copyright (c) 2012 Dan Winship
#   Copyright (c) 2015 Bastien ROUCARIES
# 	Copyright (c) 2018 sociomantic labs GmbH
#
#   This library is free software; you can redistribute it and/or modify it
#   under the terms of the GNU Lesser General Public License as published by
#   the Free Software Foundation; either version 2.1 of the License, or (at
#   your option) any later version.
#
#   This library is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
#   General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program. If not, see <https://www.gnu.org/licenses/>.

#serial 25

# SYNOPSIS
#
#   AX_LLVM_COVERAGE([coverage tool])
#
# DESCRIPTION
#
#   Checks for clang compatible code coverage tools either llvm-cov or a
#   user-specified tool (via --with-cov-tool).
#   Tests whether --coverage compile flag works and the code-coverage info file
#   can be generated. Sets
#
#     COV_TOOL to path of a wrapper script for the llvm coverage tool

AC_DEFUN([AX_LLVM_COVERAGE_REPORT], [
	AC_REQUIRE([AX_LLVM_COVERAGE_CAPTURE])

	dnl capture the code-coverage
	lcov --directory ./ \
			--base-directory ./ \
			--gcov-tool "./llvm-gcov.sh" \
			--capture -o conftest.info >/dev/null 2>&1
	ls -la conftest.info >/dev/null 2>&1
	generated_file_cov_info="$?"
	AC_LANG_POP([C++])

	dnl check that the coverage info file was generated
	AS_IF([test "$generated_file_cov_info" != "0"],
		[AC_MSG_ERROR([coverage for clang not supported])
	])
	AC_MSG_RESULT([yes]);
])

AC_DEFUN([AX_LLVM_COVERAGE_CAPTURE], [
	_COV_TOOL_BASENAME=$(basename $1)

	AS_IF([ [[[ $_COV_TOOL_BASENAME =~ ^llvm-cov ]]] ],
		dnl check if the executable specified with --with-cov-tool is in PATH
		[AC_CHECK_PROG([_LCOV_TOOL],
			[$_COV_TOOL_BASENAME],
			[$_COV_TOOL_BASENAME])
		dnl check if the full path to the llvm-cov tool is executable
		AS_IF([test -z "$_LCOV_TOOL"],
			[AS_IF([AS_EXECUTABLE_P("$1")],
				[AC_SUBST([_LCOV_TOOL], [$1])])
			])],
		dnl check if the default llvm-cov
		[AC_CHECK_PROG([_LCOV_TOOL], [llvm-cov], [llvm-cov], [])
	])

	AS_IF([test -z "$_LCOV_TOOL"],
		[AC_MSG_ERROR([Found no valid llvm coverage tool])
	])

	# try to compile with the found llvm cov tool
	AC_MSG_CHECKING([coverage support with coverage tool: $_LCOV_TOOL])
	AC_LANG_PUSH([C++])dnl
	AC_LANG_CONFTEST([AC_LANG_SOURCE([LLVM_COVERAGE_TEST])])

		dnl generate wrapper script for using the coverage tool on gcov compatible
		dnl output
	AX_PRINT_TO_FILE([llvm-gcov.sh],
[#!/bin/bash
exec $_LCOV_TOOL gcov "\$\@"
		])
	chmod +x llvm-gcov.sh

	dnl compile with code coverage
	$CXX -g -O0 --coverage conftest.cpp -o conftest >/dev/null 2>&1
	./conftest >/dev/null 2>&1

	AC_SUBST([COV_TOOL], "\${abs_builddir}/llvm-gcov.sh")
])

# SYNOPSIS
#
#   AX_GCC_COVERAGE_CAPTURE([coverage tool])
#
# DESCRIPTION
#
#   Checks for gcc compatible coverage tools gcov or a user-specified tool
#   (via --with-cov-tool). Sets
#
#     COV_TOOL to name/path of the gcov tool

AC_DEFUN([AX_GCC_COVERAGE_CAPTURE], [
	_LCOV_TOOL=$(basename $1)
	AC_CHECK_TOOL([COV_TOOL],
	[$_LCOV_TOOL])
	AS_IF([test -z "$COV_TOOL"],
		[AS_IF([AS_EXECUTABLE_P("$1")],
			[AC_SUBST([COV_TOOL], [$1])])
		])

	AS_IF([test -z "$COV_TOOL"],
		[AC_MSG_ERROR([gcov is needed to do coverage])
		])
])

AC_DEFUN([AX_CODE_COVERAGE_ADD_SUPPORT_COMPILE], [
	AC_MSG_CHECKING([Setting compiler clags])
	AS_IF([ [[[ "$CC" =~ "gcc" ]]] ], [
		_CODE_COVERAGE_LIBS=-lgcov
		_CODE_COVERAGE_CFLAGS="-fprofile-arcs -ftest-coverage"
		_CODE_COVERAGE_CXXFLAGS="-fprofile-arcs -ftest-coverage"
	],
	[AS_IF([ [[[ "$CC" =~ "clang" ]]] ], [
		_CODE_COVERAGE_CFLAGS=-coverage
		_CODE_COVERAGE_CXXFLAGS=-coverage
		])
	])
	AC_MSG_RESULT([done])
])

AC_DEFUN([AX_CODE_COVERAGE_ADD_SUPPORT_CAPTURE],
[
	AC_MSG_CHECKING([Adding support for coverage capture])
	AS_IF([ [[[ "$CC" =~ "gcc" ]]] ], [
		AX_GCC_COVERAGE_CAPTURE([$1])
	], [
		AS_IF([ [[[ "$CC" =~ "clang" ]]] ], [
			AX_LLVM_COVERAGE_CAPTURE([$1])

		])
	])
	AC_MSG_RESULT([yes])
])

AC_DEFUN([AX_CODE_COVERAGE_ADD_SUPPORT_REPORT], [
	AC_MSG_CHECKING([Adding support for coverage reports])
	AS_IF([ [[[ "$CC" =~ "clang" ]]] ], [
		AX_LLVM_COVERAGE_REPORT([$1])

	])
	AC_MSG_RESULT([yes])
])

AC_DEFUN([AX_CODE_COVERAGE],[
	AC_REQUIRE([AC_PROG_SED])

	dnl allow to override coverage tool location
	AC_ARG_WITH([cov-tool],
		[AS_HELP_STRING([--with-cov-tool[=COV_TOOL]], [use given COV_TOOL for coverage (COV_TOOL=gcov).])],
		[_cov_tool=$with_cov_tool],
		[_cov_tool=gcov])

	dnl Check for --with-coverage-support
	AC_ARG_WITH([coverage-support],
		[AS_HELP_STRING([--with-coverage-support[=COV_SUPPORT]], [use given COV_SUPPORT for coverage support level [compile,capture,report] (COV_SUPPORT=no).])],
		[_coverage_support=$with_coverage_support],
		[_coverage_support=no])

	AC_MSG_CHECKING([code coverage support])
	AS_CASE([$_coverage_support],
		[compile], [],
		[capture], [],
		[report], [],
		[no], [],
		[AC_MSG_ERROR([invalid argument for --with_coverage_support: $_coverage_support ])])
	AC_MSG_RESULT([$_coverage_support])

	AM_CONDITIONAL([CODE_COVERAGE_ENABLED], [test x$_coverage_support != xno])
	AC_SUBST([CODE_COVERAGE_ENABLED])

	AS_IF([ test "$_coverage_support" != "no" ], [

		AX_CODE_COVERAGE_ADD_SUPPORT_COMPILE([$_cov_tool])

		AC_MSG_RESULT([Using cov tool: $_cov_tool])

		AS_IF([ test "x$_coverage_support" = "xcapture" ], [
			AX_CODE_COVERAGE_ADD_SUPPORT_CAPTURE([$_cov_tool])
		])

		AS_IF([ test "x$_coverage_support" = "xreport" ], [
			AC_CHECK_PROG([LCOV], [lcov], [lcov])

			AS_IF([ test -z "$LCOV" ], [
				AC_MSG_ERROR([To enable code coverage reporting you must have lcov installed])
			])

			AC_CHECK_PROG([GENHTML], [genhtml], [genhtml])

			AS_IF([ test -z "$GENHTML" ], [
				AC_MSG_ERROR([Could not find genhtml from the lcov package])
			])

			AX_CODE_COVERAGE_ADD_SUPPORT_REPORT([$_cov_tool])
		])


		dnl Build the code coverage flags
		dnl Define CODE_COVERAGE_LDFLAGS for backwards compatibility
		CODE_COVERAGE_CPPFLAGS="-DNDEBUG"
		CODE_COVERAGE_CFLAGS="-O0 -g $_CODE_COVERAGE_CFLAGS"
		CODE_COVERAGE_CXXFLAGS="-O0 -g $_CODE_COVERAGE_CXXFLAGS"
		CODE_COVERAGE_LIBS="$_CODE_COVERAGE_LIBS"
		CODE_COVERAGE_LDFLAGS="$CODE_COVERAGE_LIBS"

		AC_SUBST([CODE_COVERAGE_CPPFLAGS])
		AC_SUBST([CODE_COVERAGE_CFLAGS])
		AC_SUBST([CODE_COVERAGE_CXXFLAGS])
		AC_SUBST([CODE_COVERAGE_LIBS])
		AC_SUBST([CODE_COVERAGE_LDFLAGS])

		[CODE_COVERAGE_RULES_CHECK='
	-$(A''M_V_at)$(MAKE) $(AM_MAKEFLAGS) -k check
	$(A''M_V_at)$(MAKE) $(AM_MAKEFLAGS) code-coverage-capture
']
		[CODE_COVERAGE_RULES_CLEAN='
clean: code-coverage-clean
distclean: code-coverage-clean
code-coverage-clean:
	-find . \( -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" \) -delete
	-@rm -rf $(abs_builddir)/coverage
']
		AS_CASE([$_coverage_support],
			[compile], [
				CODE_COVERAGE_RULES_CHECK=''
			],
			[capture], [
				[CODE_COVERAGE_RULES_CLEAN='
clean: code-coverage-clean
distclean: code-coverage-clean
code-coverage-clean:
	-find . \( -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" \) -delete
']
				[CODE_COVERAGE_RULES_CAPTURE='
	$(abs_srcdir)/beaver/bin/codecov-bash --help
	$(abs_srcdir)/beaver/bin/codecov-bash -R $(abs_srcdir) -x $(COV_TOOL) -d -Z
']
			],
			[report], [
				[CODE_COVERAGE_RULES_CAPTURE='
	$(code_coverage_v_lcov_cap)$(LCOV) $(code_coverage_quiet) $(addprefix --directory ,$(CODE_COVERAGE_DIRECTORY)) --capture --output-file "$(CODE_COVERAGE_OUTPUT_FILE).tmp" --test-name "$(call code_coverage_sanitize,$(PACKAGE_NAME)-$(PACKAGE_VERSION))" --no-checksum --compat-libtool $(CODE_COVERAGE_LCOV_SHOPTS) $(CODE_COVERAGE_LCOV_OPTIONS)
	$(code_coverage_v_lcov_ign)$(LCOV) $(code_coverage_quiet) $(addprefix --directory ,$(CODE_COVERAGE_DIRECTORY)) --remove "$(CODE_COVERAGE_OUTPUT_FILE).tmp" "/tmp/*" $(CODE_COVERAGE_IGNORE_PATTERN) --output-file "$(CODE_COVERAGE_OUTPUT_FILE)" $(CODE_COVERAGE_LCOV_SHOPTS) $(CODE_COVERAGE_LCOV_RMOPTS)
	-@rm -f $(CODE_COVERAGE_OUTPUT_FILE).tmp
	$(code_coverage_v_genhtml)LANG=C $(GENHTML) $(code_coverage_quiet) $(addprefix --prefix ,$(CODE_COVERAGE_DIRECTORY)) --output-directory "$(CODE_COVERAGE_OUTPUT_DIRECTORY)" --title "$(PACKAGE_NAME)-$(PACKAGE_VERSION) Code Coverage" --legend --show-details "$(CODE_COVERAGE_OUTPUT_FILE)" $(CODE_COVERAGE_GENHTML_OPTIONS)
	@echo "file://$(abs_builddir)/$(CODE_COVERAGE_OUTPUT_DIRECTORY)/index.html"
']

				[CODE_COVERAGE_RULES_CLEAN='
clean: code-coverage-clean
distclean: code-coverage-clean
code-coverage-clean:
	-$(LCOV) --directory $(top_builddir) -z
	-rm -rf $(CODE_COVERAGE_OUTPUT_FILE) $(CODE_COVERAGE_OUTPUT_FILE).tmp $(CODE_COVERAGE_OUTPUT_DIRECTORY)
	-find . \( -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" \) -delete
	-@rm -rf $(abs_builddir)/coverage
']
			],
		[])
	], [
		[CODE_COVERAGE_RULES_CHECK='
	@echo "Need to reconfigure with --with-coverage-support=(compile|capture|report)"
	']
		CODE_COVERAGE_RULES_CAPTURE="$CODE_COVERAGE_RULES_CHECK"
		CODE_COVERAGE_RULES_CLEAN=''
	])

[CODE_COVERAGE_RULES='
# Code coverage
#
# Optional:
#  - CODE_COVERAGE_DIRECTORY: Top-level directory for code coverage reporting.
#    Multiple directories may be specified, separated by whitespace.
#    (Default: $(top_builddir))
#  - CODE_COVERAGE_OUTPUT_FILE: Filename and path for the .info file generated
#    by lcov for code coverage. (Default:
#    $(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage.info)
#  - CODE_COVERAGE_OUTPUT_DIRECTORY: Directory for generated code coverage
#    reports to be created. (Default:
#    $(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage)
#  - CODE_COVERAGE_BRANCH_COVERAGE: Set to 1 to enforce branch coverage,
#    set to 0 to disable it and leave empty to stay with the default.
#    (Default: empty)
#  - CODE_COVERAGE_LCOV_SHOPTS_DEFAULT: Extra options shared between both lcov
#    instances. (Default: based on $CODE_COVERAGE_BRANCH_COVERAGE)
#  - CODE_COVERAGE_LCOV_SHOPTS: Extra options to shared between both lcov
#    instances. (Default: $CODE_COVERAGE_LCOV_SHOPTS_DEFAULT)
#  - CODE_COVERAGE_LCOV_OPTIONS_COV_TOOL_PATH: --gcov-tool pathtogcov
#  - CODE_COVERAGE_LCOV_OPTIONS_DEFAULT: Extra options to pass to the
#    collecting lcov instance. (Default: $CODE_COVERAGE_LCOV_OPTIONS_COV_TOOL_PATH)
#  - CODE_COVERAGE_LCOV_OPTIONS: Extra options to pass to the collecting lcov
#    instance. (Default: $CODE_COVERAGE_LCOV_OPTIONS_DEFAULT)
#  - CODE_COVERAGE_LCOV_RMOPTS_DEFAULT: Extra options to pass to the filtering
#    lcov instance. (Default: empty)
#  - CODE_COVERAGE_LCOV_RMOPTS: Extra options to pass to the filtering lcov
#    instance. (Default: $CODE_COVERAGE_LCOV_RMOPTS_DEFAULT)
#  - CODE_COVERAGE_GENHTML_OPTIONS_DEFAULT: Extra options to pass to the
#    genhtml instance. (Default: based on $CODE_COVERAGE_BRANCH_COVERAGE)
#  - CODE_COVERAGE_GENHTML_OPTIONS: Extra options to pass to the genhtml
#    instance. (Default: $CODE_COVERAGE_GENHTML_OPTIONS_DEFAULT)
#  - CODE_COVERAGE_IGNORE_PATTERN: Extra glob pattern of files to ignore
#
# The generated report will be titled using the $(PACKAGE_NAME) and
# $(PACKAGE_VERSION). In order to add the current git hash to the title,
# use the git-version-gen script, available online.

# Optional variables
CODE_COVERAGE_DIRECTORY ?= $(top_builddir)
CODE_COVERAGE_OUTPUT_FILE ?= $(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage.info
CODE_COVERAGE_OUTPUT_DIRECTORY ?= $(PACKAGE_NAME)-$(PACKAGE_VERSION)-coverage
CODE_COVERAGE_BRANCH_COVERAGE ?=
CODE_COVERAGE_LCOV_SHOPTS_DEFAULT ?= $(if $(CODE_COVERAGE_BRANCH_COVERAGE),\
--rc lcov_branch_coverage=$(CODE_COVERAGE_BRANCH_COVERAGE))
CODE_COVERAGE_LCOV_SHOPTS ?= $(CODE_COVERAGE_LCOV_SHOPTS_DEFAULT)
CODE_COVERAGE_LCOV_OPTIONS_COV_TOOL_PATH ?= --gcov-tool "$(COV_TOOL)"
CODE_COVERAGE_LCOV_OPTIONS_DEFAULT ?= $(CODE_COVERAGE_LCOV_OPTIONS_COV_TOOL_PATH)
CODE_COVERAGE_LCOV_OPTIONS ?= $(CODE_COVERAGE_LCOV_OPTIONS_DEFAULT)
CODE_COVERAGE_LCOV_RMOPTS_DEFAULT ?=
CODE_COVERAGE_LCOV_RMOPTS ?= $(CODE_COVERAGE_LCOV_RMOPTS_DEFAULT)
CODE_COVERAGE_GENHTML_OPTIONS_DEFAULT ?=\
$(if $(CODE_COVERAGE_BRANCH_COVERAGE),\
--rc genhtml_branch_coverage=$(CODE_COVERAGE_BRANCH_COVERAGE))
CODE_COVERAGE_GENHTML_OPTIONS ?= $(CODE_COVERAGE_GENHTML_OPTIONS_DEFAULT)
CODE_COVERAGE_IGNORE_PATTERN ?=

GITIGNOREFILES ?=
GITIGNOREFILES += $(CODE_COVERAGE_OUTPUT_FILE) $(CODE_COVERAGE_OUTPUT_DIRECTORY)

code_coverage_v_lcov_cap = $(code_coverage_v_lcov_cap_$(V))
code_coverage_v_lcov_cap_ = $(code_coverage_v_lcov_cap_$(AM_DEFAULT_VERBOSITY))
code_coverage_v_lcov_cap_0 = @echo "  LCOV   --capture"\
 $(CODE_COVERAGE_OUTPUT_FILE);
code_coverage_v_lcov_ign = $(code_coverage_v_lcov_ign_$(V))
code_coverage_v_lcov_ign_ = $(code_coverage_v_lcov_ign_$(AM_DEFAULT_VERBOSITY))
code_coverage_v_lcov_ign_0 = @echo "  LCOV   --remove /tmp/*"\
 $(CODE_COVERAGE_IGNORE_PATTERN);
code_coverage_v_genhtml = $(code_coverage_v_genhtml_$(V))
code_coverage_v_genhtml_ = $(code_coverage_v_genhtml_$(AM_DEFAULT_VERBOSITY))
code_coverage_v_genhtml_0 = @echo "  GEN   " $(CODE_COVERAGE_OUTPUT_DIRECTORY);
code_coverage_quiet = $(code_coverage_quiet_$(V))
code_coverage_quiet_ = $(code_coverage_quiet_$(AM_DEFAULT_VERBOSITY))
code_coverage_quiet_0 = --quiet

# sanitizes the test-name: replaces with underscores: dashes and dots
code_coverage_sanitize = $(subst -,_,$(subst .,_,$(1)))

# Use recursive makes in order to ignore errors during check
check-code-coverage:'"$CODE_COVERAGE_RULES_CHECK"'

# Capture code coverage data
code-coverage-capture: code-coverage-capture-hook'"$CODE_COVERAGE_RULES_CAPTURE"'

# Hook rule executed before code-coverage-capture, overridable by the user
code-coverage-capture-hook:

'"$CODE_COVERAGE_RULES_CLEAN"'

A''M_DISTCHECK_CONFIGURE_FLAGS ?=
A''M_DISTCHECK_CONFIGURE_FLAGS += --disable-code-coverage

.PHONY: check-code-coverage code-coverage-capture code-coverage-capture-hook code-coverage-clean
']
	AC_SUBST([CODE_COVERAGE_RULES])
	m4_ifdef([_AM_SUBST_NOTMAKE], [_AM_SUBST_NOTMAKE([CODE_COVERAGE_RULES])])
])

m4_define([LLVM_COVERAGE_TEST], [[

	#include <stdio.h>
	int main(int argc, char **argv) {
		if (argc > 1) {
			printf("GOT: %s\n", argv[1]);
		}
		return 0;
	}

]])
