#pragma once


#include <type_traits>
#include <stdexcept>

#include <mathfu/vector.h>


namespace inl {
namespace gxeng {




// Available semantics.
enum eVertexElementSemantic {
	POSITION,
	NORMAL,
};



// Describes a vertex element, which has 2 properties: semantic and index.
template <eVertexElementSemantic Semantic, int Index>
class VertexElement {
public:
	static constexpr eVertexElementSemantic semantic = Semantic;
	static constexpr int index = Index;
};

template <int Index>
using Position = VertexElement<POSITION, Index>;

template <int Index>
using Normal = VertexElement<NORMAL, Index>;



// Stores values related to a certain semantic. The type of the values and list of indices is specified.
// TODO: optimize for Indices... that form a sequence [0,1,2,3 ...]
template <class T, int... Indices>
class VertexPartData {
private:
	static constexpr int count = sizeof...(Indices);
	static constexpr int table[count] = { Indices... };
protected:
	T values[count]; //
public:
	T& operator[](int index) {
		for (int lookupIdx = 0; lookupIdx < count; ++lookupIdx) {
			if (table[lookupIdx] == index) {
				return values[lookupIdx];
			}
		}
		throw std::invalid_argument("Index not found.");
	}
	const T& operator[](int index) const {
		for (int lookupIdx = 0; lookupIdx < count; ++lookupIdx) {
			if (table[lookupIdx] == index) {
				return values[lookupIdx];
			}
		}
		throw std::invalid_argument("Index not found.");
	}
};
// Just the definition of statics members
template <class T, int... Indices>
constexpr int VertexPartData<T, Indices...>::table[count];



// List of semantics.
template <eVertexElementSemantic... Semantics>
class SemanticList {};

template <class T, class U>
class ConcatSemanticList;

template <eVertexElementSemantic... Semantics1, eVertexElementSemantic... Semantics2>
class ConcatSemanticList<SemanticList<Semantics1...>, SemanticList<Semantics2...>> {
public:
	using type = SemanticList<Semantics1..., Semantics2...>;
};



// Decides if a SemanticList contains a given Semantic.
template <eVertexElementSemantic Semantic, class List>
class ContainsSemantic;

template <eVertexElementSemantic Semantic, eVertexElementSemantic... List>
class ContainsSemanticHelper;

template <eVertexElementSemantic Semantic, eVertexElementSemantic Head, eVertexElementSemantic... List>
class ContainsSemanticHelper<Semantic, Head, List...> {
public:
	static constexpr bool value = (Semantic == Head) || ContainsSemanticHelper<Semantic, List...>::value;
};

template <eVertexElementSemantic Semantic>
class ContainsSemanticHelper<Semantic> {
public:
	static constexpr bool value = false;
};

template <eVertexElementSemantic Semantic, eVertexElementSemantic... List>
class ContainsSemantic<Semantic, SemanticList<List...>> {
public:
	static constexpr bool value = ContainsSemanticHelper<Semantic, List...>::value;
};



// Used to conditionally inherit from.
class EmptyClass {};



// Represents a list of arbitrary types.
template <class... Types>
class TypeList {};

template <class T, class U>
class ConcatTypeList;

template <class... Types1, class... Types2>
class ConcatTypeList<TypeList<Types1...>, TypeList<Types2...>> {
public:
	using type = TypeList<Types1..., Types2...>;
};



// Vertex parts
template <eVertexElementSemantic Semantic>
class VertexPart {
public:
	virtual ~VertexPart() = default;
};

template <eVertexElementSemantic Semantic, int... Indices>
class VertexPartImpl;

// Position
template <>
class VertexPart<POSITION> {
public:
	virtual ~VertexPart() = default;
	using DataType = mathfu::Vector<float, 3>;
	virtual DataType& GetPosition(int index) = 0;
	virtual const DataType& GetPosition(int index) const = 0;
};

template <int Index, int... Indices>
class VertexPartImpl<POSITION, Index, Indices...> : public VertexPart<POSITION> {
public:
	DataType& GetPosition(int index) override { return positions[index]; }
	const DataType& GetPosition(int index) const override { return positions[index]; }
	VertexPartData<DataType, Index, Indices...> positions;
};

template <int Index>
class VertexPartImpl<POSITION, Index> : public VertexPart<POSITION> {
public:
	DataType& GetPosition(int index) override { return positions[index]; }
	const DataType& GetPosition(int index) const override { return positions[index]; }
	VertexPartData<DataType, Index> positions;
	DataType& position = positions[Index];
};

// Normal
template <>
class VertexPart<NORMAL> {
public:
	virtual ~VertexPart() = default;
	using DataType = mathfu::Vector<float, 3>;
	virtual DataType& GetNormal(int index) = 0;
	virtual const DataType& GetNormal(int index) const = 0;
};

template <int Index, int... Indices>
class VertexPartImpl<NORMAL, Index, Indices...> : public VertexPart<NORMAL> {
public:
	DataType& GetNormal(int index) override { return normals[index]; }
	const DataType& GetNormal(int index) const override { return normals[index]; }
	VertexPartData<DataType, Index, Indices...> normals;
};

template <int Index>
class VertexPartImpl<NORMAL, Index> : public VertexPart<NORMAL> {
public:
	DataType& GetNormal(int index) override { return normals[index]; }
	const DataType& GetNormal(int index) const override { return normals[index]; }
	VertexPartData<DataType, Index> normals;
	DataType& normal = normals[Index];
};



// Given a Semantic and a list of VertexElements, filters out VertexElements that have the specified Semantic.
// Inherits from the VertexPart which implements the filter Semantic
template <class TypeListOfChosenElements, eVertexElementSemantic Filter, class... RemainingElements>
class ElementFilterHelper;

template <class TypeListOfChosenElements, eVertexElementSemantic Filter, class HeadElement, class... RemainingElements>
class ElementFilterHelper<TypeListOfChosenElements, Filter, HeadElement, RemainingElements...>
	: public ElementFilterHelper<
		typename std::conditional<HeadElement::semantic == Filter,
								  typename ConcatTypeList<TypeListOfChosenElements, TypeList<HeadElement>>::type,
								  TypeListOfChosenElements>::type,
		Filter,
		RemainingElements...>
{};

// Extracts Elements from TypeList of VertexElements.
// Inherits from implementor of corresponding vertex part.
template <class... Elements, eVertexElementSemantic Semantic>
class ElementFilterHelper<TypeList<Elements...>, Semantic>
	: public VertexPartImpl<Semantic, Elements::index...>
{};

template <eVertexElementSemantic Filter, class... Elements>
class ElementFilter;

template <eVertexElementSemantic Filter, eVertexElementSemantic... ElementSemantics, int... ElementIndices>
class ElementFilter<Filter, VertexElement<ElementSemantics, ElementIndices>...>
	: public ElementFilterHelper<TypeList<>, Filter, VertexElement<ElementSemantics, ElementIndices>...>
{};



// Helper class to form a vertex.
// Groups vertex elements by semantic, then inherits from the container for that semantic.
template <class UsedSemantics, class... Elements>
class VertexHelper;

template <class UsedSemantics, eVertexElementSemantic FirstSemantic, int FirstIndex>
class VertexHelper<UsedSemantics, VertexElement<FirstSemantic, FirstIndex>>
	: virtual public std::conditional<ContainsSemantic<FirstSemantic, UsedSemantics>::value,
	EmptyClass,
	ElementFilter<FirstSemantic, VertexElement<FirstSemantic, FirstIndex>>>::type
{};

template <class UsedSemantics, eVertexElementSemantic FirstSemantic, int FirstIndex, eVertexElementSemantic... Semantics, int... Indices>
class VertexHelper<UsedSemantics, VertexElement<FirstSemantic, FirstIndex>, VertexElement<Semantics, Indices>...>
	: virtual public std::conditional<ContainsSemantic<FirstSemantic, UsedSemantics>::value,
	EmptyClass,
	ElementFilter<FirstSemantic, VertexElement<FirstSemantic, FirstIndex>, VertexElement<Semantics, Indices>...>>::type,
	public VertexHelper<typename ConcatSemanticList<UsedSemantics, SemanticList<FirstSemantic>>::type, VertexElement<Semantics, Indices>...>
{};


class VertexBase {
public:
	virtual ~VertexBase() {}
};


// Actual vertex class
template <class... Elements>
class Vertex : public VertexBase, public VertexHelper<SemanticList<>, Elements...>
{};




} // namespace gxeng
} // namespace inl