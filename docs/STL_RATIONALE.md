# STL Choices and Alternatives

This note explains why the project uses selected C++ standard library (STL) facilities and why common alternatives were not chosen. It reflects the code as it currently exists.

## `std::vector` for query results and byte buffers

The SQLite repositories return lists of assignments, courses, submissions, and audit entries as `std::vector<T>`. A vector stores elements in order, grows as rows are read, and manages its own memory. This suits database query results because the number of rows is not known in advance. `PasswordHasher.cpp` also uses vectors for message and salt bytes so their lengths can be determined at runtime.

Alternatives considered:

- A fixed-size array has low overhead, but requires choosing a maximum size and handling overflow or truncation. That is a poor fit for query results whose size depends on database contents.
- A linked list grows dynamically, but needs per-element allocations and has less convenient iteration and locality for these simple read-and-return lists.
- A custom dynamic array could provide similar behavior, but would require implementing and maintaining allocation, resizing, copying, and bounds behavior that `std::vector` already provides.

For small fixed-size data, such as the 16-byte password salt, `std::array` could also be reasonable. The current vector-based helper accepts a runtime byte count, so one implementation handles both the salt and SHA-256 padded message.

## `std::optional` for possibly absent records and values

Repository lookups return `std::optional<T>` when a record may not exist. The optional represents both outcomes in the return type: a value was found, or no value was found. `std::optional<double>` is also used when moderation may provide a replacement mark or leave it unchanged.

Alternatives considered:

- Returning a default-constructed object would make `not found` ambiguous with a real object containing default values.
- Returning a pointer or reference can signal absence with `nullptr`, but introduces lifetime and ownership questions. Repository results here are values, not references into a repository-owned object.
- A separate boolean plus an output parameter works, but spreads one logical result over two parameters and is easier to misuse.

## `std::unique_ptr` for polymorphic users

User lookup and factory methods return `std::unique_ptr<User>`. Users have derived types such as `Student`, `Lecturer`, and `Administrator`; a pointer to the base class allows the concrete type to vary. `unique_ptr` also makes ownership explicit and automatically destroys the object when its owner goes away.

Alternatives considered:

- A raw owning pointer requires every caller to remember `delete`, which risks leaks and double deletion.
- `std::shared_ptr` enables shared ownership, but the current design transfers a single clear owner and does not need reference counting.
- Returning a base `User` value would slice derived-class data and behavior.

## `std::string` for text

The application uses `std::string` for names, usernames, SQL text, timestamps, passwords, and feedback. These values vary in length and need operations such as concatenation, substring extraction, and comparison. `std::string` handles storage and null termination details without fixed text limits.

A fixed character buffer would require an explicit size and careful bounds handling. C-style strings would also require manual memory and termination management. Neither offers a useful benefit for these application fields.

## `std::move` when transferring owned strings

The user constructors take string values and move them into member fields. This supports efficient construction when the caller supplies a temporary or a value it no longer needs, while retaining a simple value-based constructor interface. A copy would also be correct, but may allocate and copy the string contents unnecessarily.

## `std::max` in the late penalty strategy

`LatePenaltyStrategy` uses `std::max(0.0, rawMark - pen)` to ensure the adjusted mark cannot fall below zero. The standard algorithm expresses this lower-bound rule directly and avoids a hand-written branch. A simple conditional would be equally suitable for this one comparison if algorithms are restricted.

## `std::filesystem` for paths

The program uses `std::filesystem` to work with the schema file path. It provides path-aware file operations without relying on manual string concatenation or platform-specific separators. This is only used in the startup code.

## If STL containers or algorithms are restricted

The current implementation uses both `std::vector` and the `<algorithm>` facility `std::max`. A restriction on containers and algorithms would therefore require changes: vectors would need an allowed replacement, and `std::max` could become a direct conditional. `std::string`, `std::optional`, `std::unique_ptr`, `std::move`, and `std::filesystem` are not containers or algorithms, so they would remain available under that specific restriction.

For dynamically sized query results, a custom growable collection preserves flexibility but adds implementation and maintenance work. Fixed-size arrays are simpler but impose capacity limits. The assignment or course rule should determine whether a custom collection is allowed; this rationale does not override that rule.
