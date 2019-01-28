#include "TestCommandMetadata.h"
#include "interfaces/ITestUtility.h"

#include "../Module.h"

namespace WPEFramework {

class TestCommandBase : public Exchange::ITestUtility::ICommand {
    public:
#if 0
        struct Parameter {
            Parameter(const string& name, const string& type, const string& comments)
                : _name(name)
                , _type(type)
                , _comments(comments)
            {}
            string _name;
            string _type;
            string _comments;
        };
#endif
        class SignatureBuilder {
            public:
                SignatureBuilder(const SignatureBuilder&) = delete;
                SignatureBuilder& operator=(const SignatureBuilder&) = delete;

                SignatureBuilder(const TestCore::TestCommandSignature::Parameter& returnValue)
                    : _jsonSignature()
                {
                    _jsonSignature.Input.Add(returnValue);
                }

                SignatureBuilder& AddOutParameter(const TestCore::TestCommandSignature::Parameter& parameter)
                {
                    _jsonSignature.Output.Add(parameter);
                    return *this;
                }

                virtual ~SignatureBuilder() = default;

            private:

                friend class TestCommandBase;
                string ToString() const
                {
                    string outString;
                    _jsonSignature.ToString(outString);

                    return outString;
                }

                TestCore::TestCommandSignature _jsonSignature;
        };

        class DescriptionBuilder {
            public:
                DescriptionBuilder(const DescriptionBuilder&) = delete;
                DescriptionBuilder& operator=(const DescriptionBuilder&) = delete;

                DescriptionBuilder(const string& description)
                    : _jsonDescription()
                {
                    _jsonDescription.Description = description;
                }

                virtual ~DescriptionBuilder() = default;

            private:

                friend class TestCommandBase;
                string ToString() const
                {
                    string outString;
                    _jsonDescription.ToString(outString);

                    return outString;
                }

                TestCore::TestCommandDescription _jsonDescription;
        };

    private:
        TestCommandBase(const TestCommandBase&) = delete;
        TestCommandBase& operator=(const TestCommandBase&) = delete;

    public:
        TestCommandBase(const DescriptionBuilder& description, const SignatureBuilder& signature)
            : Exchange::ITestUtility::ICommand()
            , _signature(signature.ToString())
            , _description(description.ToString())
        { }

        virtual ~TestCommandBase() = default;

    public:
        // ICommand methods
        string Description() const final
        {
            return _description;
        }

        string Signature() const final
        {
            return _signature;
        }

    private:
        string _description;
        string _signature;
};
} // namespace WPEFramework
