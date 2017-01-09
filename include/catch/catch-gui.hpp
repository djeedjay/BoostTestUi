//  (C) Copyright Gert-Jan de Vos 2012.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://boosttestui.wordpress.com/ for the boosttestui home page.

#ifdef CATCH_CONFIG_MAIN

#undef CATCH_CONFIG_MAIN
#define CATCH_CONFIG_RUNNER
#define CATCH_GUI_CONFIG_MAIN

#endif // CATCH_CONFIG_MAIN

#include "catch.hpp"

#ifdef CATCH_GUI_CONFIG_MAIN

namespace Catch {

    struct BoostTestUiReporter : StreamingReporterBase
	{
        BoostTestUiReporter(ReporterConfig const& _config) :
			StreamingReporterBase(_config),
            m_headerPrintedForThisSection(false)
        {
        }

        virtual ~BoostTestUiReporter() CATCH_OVERRIDE;

        static std::string getDescription()
		{
            return "BoostTestUi test result reporter";
        }

        virtual void skipTest(TestCaseInfo const& testInfo) CATCH_OVERRIDE
		{
            stream  << "#TestIgnored " << testInfo.name << "\n";
        }

        virtual void noMatchingTestCases(std::string const& /* spec */) CATCH_OVERRIDE
		{
		}

        virtual void testGroupStarting(GroupInfo const& groupInfo) CATCH_OVERRIDE
		{
            StreamingReporterBase::testGroupStarting( groupInfo );
			stream << "#RunStarted " << groupInfo.groupsCounts << "\n";
        }

        virtual void testGroupEnded(TestGroupStats const& testGroupStats) CATCH_OVERRIDE
		{
            StreamingReporterBase::testGroupEnded( testGroupStats );
            stream << "#RunFinished\n";
        }

        virtual void assertionStarting(AssertionInfo const&) CATCH_OVERRIDE
		{
        }

        virtual bool assertionEnded(AssertionStats const& assertionStats) CATCH_OVERRIDE
		{
            AssertionResult const& result = assertionStats.assertionResult;
            if (!result.isOk() || m_config->includeSuccessfulResults())
			{
                std::ostringstream msg;
                if (!m_headerPrintedForThisSection)
                    printSectionHeader(msg);
                m_headerPrintedForThisSection = true;

                msg << result.getSourceInfo() << "\n";

                switch (result.getResultType())
				{
                case ResultWas::ExpressionFailed:
                    msg << "expression failed";
                    break;
                case ResultWas::ThrewException:
                    msg << "unexpected exception";
                    break;
                case ResultWas::FatalErrorCondition:
                    msg << "fatal error condition";
                    break;
                case ResultWas::DidntThrowException:
                    msg << "no exception was thrown where one was expected";
                    break;
                case ResultWas::ExplicitFailure:
                    msg << "explicit failure";
                    break;

                case ResultWas::Ok:
                    msg << "PASSED";
                    break;

				case ResultWas::Info:
                    msg << "info";
                    break;

				case ResultWas::Warning:
                    msg << "warning";
                    break;

                // These cases are here to prevent compiler warnings
                case ResultWas::Unknown:
                case ResultWas::FailureBit:
                case ResultWas::Exception:
                    CATCH_NOT_IMPLEMENTED;
                }
                if (assertionStats.infoMessages.size() == 1)
                    msg << " with message:";
                if (assertionStats.infoMessages.size() > 1)
                    msg << " with messages:";
                for (std::vector<MessageInfo>::const_iterator it = assertionStats.infoMessages.begin(), itEnd = assertionStats.infoMessages.end(); it != itEnd; ++it)
                    msg << "\n  \"" << it->message << "\"";

                if (result.hasExpression())
				{
                    msg <<
                        "\n  " << result.getExpressionInMacro() << "\n"
                        "with expansion:\n" <<
                        "  " << result.getExpandedExpression() << "\n";
                }
				stream << "\n";
                stream << msg.str() << "#Assertion " << (result.isOk() ? 1 : 0) <<  "\n";
            }
            return true;
        }

        virtual void sectionStarting(SectionInfo const& sectionInfo) CATCH_OVERRIDE
		{
            m_headerPrintedForThisSection = false;
            StreamingReporterBase::sectionStarting(sectionInfo);
        }

        virtual void testCaseStarting(TestCaseInfo const& testInfo) CATCH_OVERRIDE
		{
            StreamingReporterBase::testCaseStarting(testInfo);
            stream << "#TestStarted " << testInfo.name << "\n";
        }

        virtual void testCaseEnded(TestCaseStats const& testCaseStats) CATCH_OVERRIDE
		{
            StreamingReporterBase::testCaseEnded(testCaseStats);			
            stream << "#TestFinished " << (testCaseStats.totals.testCases.allOk() ? "1" : "0") << " " << testCaseStats.testInfo.name << "\n";
        }

    private:
        void printSectionHeader(std::ostream& os)
		{
            assert(!m_sectionStack.empty());

            os  << getLineOfChars<'-'>() << "\n";
            printHeaderString(os, currentTestCaseInfo->name);

			if (m_sectionStack.size() > 1)
			{
                std::vector<SectionInfo>::const_iterator
                it = m_sectionStack.begin() + 1, // Skip first section (test case)
                itEnd = m_sectionStack.end();
                for ( ; it != itEnd; ++it)
                    printHeaderString(os, "  " + it->name);
            }
            os << getLineOfChars<'-'>() << "\n";

			SourceLineInfo lineInfo = m_sectionStack.front().lineInfo;
            if (!lineInfo.empty())
                os << lineInfo << "\n";
            os << getLineOfChars<'.'>() << "\n\n";
        }

        // if string has a : in first line will set indent to follow it on
        // subsequent lines
        void printHeaderString(std::ostream& os, std::string const& _string, std::size_t indent = 0)
		{
            std::size_t i = _string.find(": ");
            if (i != std::string::npos)
                i += 2;
            else
                i = 0;
            os << Text(_string, TextAttributes().setIndent(indent + i).setInitialIndent(indent)) << "\n";
        }

	private:
        bool m_headerPrintedForThisSection;
    };

#ifdef CATCH_IMPL
    BoostTestUiReporter::~BoostTestUiReporter()
	{
	}
#endif

    INTERNAL_CATCH_REGISTER_REPORTER("boosttestui", BoostTestUiReporter)

} // end namespace Catch

int main(int argc, char* argv[])
{
	const std::string gui_wait = "--gui_wait";
	for (int i = 1; i < argc; ++i)
	{
		if (argv[i] == gui_wait)
		{
			std::cout << "#Waiting" << std::endl;
			std::getchar();
			for (int j = i; j < argc; ++j)
				argv[j] = argv[j + 1];
			--argc;
			break;
		}
	}
    return Catch::Session().run(argc, argv);
}

#if _WIN32
extern "C" __declspec(dllexport) void unit_test_type_catch()
{
}
#endif

#endif // CATCH_GUI_CONFIG_MAIN
