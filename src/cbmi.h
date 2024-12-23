
#pragma once

#include <memory>
#include <type_traits>

namespace ClvLib {
/**
    @brief This code provides near zero cost value semantics plus runtime
           polymorphism for any type.
    @tparam ConceptImpl Injected concept definition
    @tparam ModelImpl Injected model implementation boilerplate
    @tparam TypesT Additional optional type parameters that are passed through via
            CRTP to ModelImpl
    @class ImplCBMI

    CBMI (concept based model idiom) framework, AKA 'value OOP'. This is a C++17
    implementation of the concept-based model idiom as a framework, which provides
    a way to create polymorphic objects with value semantics. The framework is based
    on the idea of using a concept definition to define the interface that the
    implementation objects must adhere to, and a model implementation to provide the
    model to implementation delegation details.

    - ConceptImpl: The concept definition is a template that defines the interface that
      the implementation objects must adhere to. It is a pure virtual class that defines
      the methods that the implementation objects must implement.
    - ModelImpl: The model implementation is a template that provides the implementation
      details for the concept. It is a class that implements delegation to the actual
      implementation objects.
    - TypesT: Additional optional type parameters that are passed through via CRTP to
      the ConceptImpl and ModelImpl templates.

    The ImplCBMI class allows you to create objects that behave polymorphically, i.e.,
    you can store objects of different types in a ImplCBMI object and access them through
    a uniform interface. This can be useful in scenarios where you need to work with
    objects of different types but want to treat them uniformly without maintaining a
    formal inheritance system.

    Example - Create the thing interface like this:

        template <typename...>
        struct thing_concept
        {
            virtual ~thing_concept() noexcept = default;
            virtual void do_something() = 0;
            virtual void do_something_else() = 0;
        };

        template <typename BaseT>
        struct thing_delegate
        {
            using BaseT::BaseT;
            void do_something()
            {
                BaseT::DelegateTo().do_something();
            }
            void do_something_else()
            {
                BaseT::DelegateTo().do_something_else();
            }
        };

    Finally we can alias it into a nicer name, which also glues everything together.

        using thing = Poly<thing_concept, thing_delegate>;

    See unit tests for complete examples. This alias reuses the delegator as both the
    interface and model. More control is possible by separating the model and delegator.
    See the 'Draw' example in the unit tests for a more complex example, the 'Duck'
    example for a simple example, and the 'Gen' example for a generic example.

    Now any type that implements the API of a 'thing' can be treated polymorphically
    without any tight coupling from a rigid inheritance tree. The assembly generated
    by this framework vs hand rolled vtable access (gcc 13 -O3 or clang 16 -O3) is
    virtually identical and so is the speed.

    NOTE: In the distant future, when C++26++ is the norm, this could be further simplified
          by using reflection to generate the delegation boilerplate. This would allow the 
          user to define the interface in a single place and have the boilerplate generated 
          automatically. 
*/
// clang-format off
template <template <typename...> typename ConceptImpl,
          template <typename...> typename ModelImpl,
          typename... TypesT>
// clang-format on
class ImplCBMI
{
    struct Concept : ConceptImpl<TypesT...>
    {
        virtual ~Concept() noexcept = default;
        virtual const Concept *Clone() const = 0;
        virtual Concept *Clone() = 0;
    };

    template <typename TypeImplT>
    struct Model;

    std::unique_ptr<Concept> mPoly;

  public: // Instance management & CRTP help
    // Construct using an instance of the implementation type
    template <typename PolyT>
    ImplCBMI(PolyT &&poly)
        requires(!std::is_same_v<ImplCBMI, std::remove_reference_t<PolyT>>)
        : mPoly(new ModelImpl<Model<PolyT>, TypesT...>(std::forward<PolyT>(poly))){};
    // Pointer-to-Poly uses partial specialization
    template <typename PolyT>
    ImplCBMI(PolyT *poly)
        : mPoly(new ModelImpl<Model<PolyT *>, TypesT...>(poly)){};
    // Copy operations
    ImplCBMI(const ImplCBMI &poly)
        : mPoly(poly.mPoly->Clone()){};
    ImplCBMI &operator=(const ImplCBMI &poly)
    {
        if (this != &poly)
        {
            auto t = poly.mPoly->Clone();
            mPoly.reset(t);
        }
        return *this;
    }
    // Move operations
    ImplCBMI(ImplCBMI &&) = default;
    ImplCBMI &operator=(ImplCBMI &&poly) = default;
    // CRTP helpers
    const Concept &DelegateTo() const noexcept
    {
        return *mPoly;
    }
    Concept &DelegateTo() noexcept
    {
        return *mPoly;
    }
};
/**
    @brief CRTP base class that will be injected into the user supplied delegator 'ModelImpl'
    @tparam ConceptImpl Injected concept definition
    @tparam ModelImpl Injected model implementation boilerplate
    @tparam TypesT Additional optional type parameters that are passed through via CRTP to ModelImpl
    @tparam TypeImplT

    The `Model` struct inside the `ImplCBMI` class template is a concrete implementation of the
    polymorphic concept defined by the `Concept` member, and it uses the ModelImpl template parameter
    via CRTP derivation to delegate the ConceptImpl interface to the underlying implementation
    object. It's used to hold and manage an object of a specific type (`TypeImplT`) while providing
    the necessary polymorphic behavior required by the `ImplCBMI` wrapper.

    In summary, the `Model` struct acts as a bridge between the type-specific implementation
    (`TypeImplT`) and the polymorphic interface defined by `ConceptImpl`. It ensures that the
    `ImplCBMI` wrapper can store and manipulate objects of different types while maintaining a
    uniform interface.
*/
// clang-format off
template <template <typename...> typename ConceptImpl,
          template <typename...> typename ModelImpl,
          typename... TypesT>
// clang-format on
template <typename TypeImplT>
struct ImplCBMI<ConceptImpl, ModelImpl, TypesT...>::Model : Concept
{
    Model(const TypeImplT &impl)
        : mObj(impl){};
    Model(TypeImplT &&impl)
        requires std::is_rvalue_reference_v<decltype(impl)>
        : mObj(std::forward<TypeImplT>(impl)){};
    Concept *Clone() const final
    {
        return new ModelImpl<Model<TypeImplT>, TypesT...>(mObj);
    }
    Concept *Clone() final
    {
        return new ModelImpl<Model<TypeImplT>, TypesT...>(mObj);
    }
    const TypeImplT &DelegateTo() const noexcept
    {
        return mObj;
    }
    TypeImplT &DelegateTo() noexcept
    {
        return mObj;
    }

  private:
    TypeImplT mObj;
};

/**
    @brief Partial specialization for TypeImplT *
    @tparam ConceptImpl
    @tparam ModelImpl
    @tparam TypesT
    @tparam TypeImplT
    @details This is a partial specialization of the ImplCBMI::Model struct template for the case
             where TypeImplT is a pointer type. This specialization is used when the implementation
             object being wrapped by the ImplCBMI class is a pointer to an object, rather than an
             object itself. In summary, this partial specialization is used to handle the case where
             the implementation object is a pointer, rather than an object itself. It simplifies the
             implementation by avoiding unnecessary copies or allocations, and provides a way to
             access the underlying implementation object through the DelegateTo functions.
*/
// clang-format off
template <template <typename...> typename ConceptImpl,
          template <typename...> typename ModelImpl,
          typename... TypesT>
template <typename TypeImplT>
struct ImplCBMI<ConceptImpl, ModelImpl, TypesT...>::Model<TypeImplT *> : Concept
{
    Model(TypeImplT *impl) : mObj(impl){};
    const Concept *Clone() const final
    {
        return new ModelImpl<Model<TypeImplT *>, TypesT...>(mObj);
    }
    Concept *Clone() final
    {
        return new ModelImpl<Model<TypeImplT *>, TypesT...>(mObj);
    }
    const TypeImplT &DelegateTo() const noexcept { return *mObj; }
    TypeImplT &DelegateTo() noexcept { return *mObj; }

  private:
    TypeImplT *mObj;
};
// clang-format on

/**
    @brief Alias for a polymorphic interface implementation using the provided concept, model, and interface.
    @tparam PolyConceptT Concept template to use for the polymorphic definition. Defines the interface
            that the actual implementation objects must adhere to, except the implementation objects
            are not required (or recommended) to be polymorphic. The concept is a pure virtual class.
    @tparam PolyInterfaceT Interface template to use for the top implementation. Provides the top level
            delegation logic and interface that calls into the Model. This is a CRTP base class that
            delegates to the internal PolyModelT/ModelImpl object. This delegation is resolved at compile
            time and the class is not required or recommended to be polymorphic.
    @tparam PolyModelT Model template to use for the implementation. Provides the implementation details
            for dispatching calls from the top level object to the actual implementation object. This
            class is not required or recommended to be polymorphic. The real polymorphism is achieved
            vis CRTP inheritance inside ImplCBMI via ImplCBMI::Model.
    @tparam TypesT Parameter pack of types to use - these are passed through to the PolyModelT and
            PolyConceptT templates via CRTP. This allows the PolyModelT and PolyConceptT templates to
            be parameterized by additional types if needed.
    @note The functionality of this framework requires a sort of double CRTP dispatch to work correctly.
          The `PolyCMI` alias template is used to create a type that models the concept defined by
          `PolyConceptT` for all types in `TypesT`. This type is not itself a polymorphic type, but it
          wraps up a polymorphic type inside a Model that delegates to the actual implementation object.
          All this should get boiled down to a vtable call at runtime with the CRTP and double dispatching
          happening at compile time. The short version is that the supplied interface CRTP derives from
          ImplCBMI, which creates an instance of Model<> that also derives from Concept, which is an alias
          for PolyConceptT. The Model<> class then delegates to the actual implementation object, often
          via the DelegateTo() CRTP convenience functions.
*/
// clang-format off
template <template <typename...> typename PolyConceptT,
          template <typename...> typename PolyInterfaceT,
          template <typename...> typename PolyModelT,
          typename... TypesT>
using PolyCMI = PolyInterfaceT<ImplCBMI<PolyConceptT, PolyModelT, TypesT...>, TypesT...>;
// clang-format on
// clang-format off
/**
    @brief Alias template for creating a polymorphic type with concept-based model idiom
    @tparam PolyConceptT The concept template to use for the polymorphic type
    @tparam PolyT The base template for the polymorphic type
    @tparam TypesT The variadic template parameter pack of types to model
    @details This alias template creates a polymorphic type using the concept-based model idiom
             (CBMI). It takes a concept template `PolyConceptT`, a base template `PolyT`, and a
             variadic pack of types `TypesT` to model. The resulting type `Poly` is an alias for
             `PolyCMI<PolyConceptT, PolyT, PolyT, TypesT...>`, which is a type that models the
             concept `PolyConceptT` for all types in `TypesT`.
*/
template <template <typename...> typename PolyConceptT,
          template <typename...> typename PolyT,
          typename... TypesT>
using Poly = PolyCMI<PolyConceptT, PolyT, PolyT, TypesT...>;
// clang-format on

} // namespace ClvLib