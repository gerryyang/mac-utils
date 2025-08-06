---
layout: post
title:  "Ast-grep in Action"
date:   2025-07-24 18:00:00 +0800
categories: Tools
---

* Do not remove this line (it will not be displayed)
{:toc}

[ast-grep](https://github.com/ast-grep/ast-grep)(sg) is a CLI tool for code structural search, lint, and rewriting.

`ast-grep` is an [abstract syntax tree](https://dev.to/balapriya/abstract-syntax-tree-ast-explained-in-plain-english-1h38) based tool to search code by pattern code. Think of it as your old-friend `grep`, but matching AST nodes instead of text. You can write patterns as if you are writing ordinary code. It will match all code that has the same syntactical structure. You can use `$` sign + upper case letters as a [wildcard](https://en.wikipedia.org/wiki/Wildcard_character), e.g. `$MATCH`, to match any single AST node. Think of it as [regular expression dot](https://regexone.com/lesson/wildcards_dot) `.`, except it is not textual.

Try the [online playground](https://ast-grep.github.io/playground.html) for a taste!


# Feature

## Search and Rewrite

`ast-grep` is a code tool for structural search and replace. It is like syntax-aware `grep`/`sed`! You can write code [patterns](https://ast-grep.github.io/guide/pattern-syntax.html) to locate and modify code, based on AST, in thousands of files, [interactively](https://ast-grep.github.io/guide/tooling-overview.html#interactive-mode).

``` bash
ast-grep -p '$A && $A()' -r '$A?.()'
```

![astgrep1](/assets/images/202507/astgrep1.png)


## Scan as Linter

`ast-grep` is a versatile and flexible tool for [linting](https://ast-grep.github.io/guide/scan-project.html) code with AST patterns. You can easily add new customized rules with [intuitive syntax](https://ast-grep.github.io/guide/rule-config.html) and enjoy pretty error reporting out of box.

``` bash
ast-grep scan
```

![astgrep2](/assets/images/202507/astgrep2.png)


## Programmatic Usage

`ast-grep` also provides [node-js binding](https://ast-grep.github.io/guide/api-usage/js-api.html) to access syntax trees programmatically. You can use jQuery like [utility methods](https://ast-grep.github.io/reference/api.html#napi) to traverse syntax tree nodes. Node API also has opt-in [type safety](https://ast-grep.github.io/blog/typed-napi.html).

``` bash
npm install @ast-grep/napi
```

![astgrep3](/assets/images/202507/astgrep3.png)



# Quick Start

You can unleash ast-grep's power at your finger tips within few keystrokes in command line!

Let's try its power of by rewriting some code in a moderately large codebase: [TypeScript](https://github.com/microsoft/TypeScript/).

Our task is to rewrite old defensive code that checks nullable nested method calls to the new shiny [optional chaining operator](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Optional_chaining) `?.`.


## Installation

First, install `ast-grep`. It is distributed by [npm](https://www.npmjs.com/package/@ast-grep/cli), [cargo](https://crates.io/crates/ast-grep), [homebrew](https://formulae.brew.sh/formula/ast-grep) and [macports](https://ports.macports.org/port/ast-grep/). You can also build it [from source](https://github.com/ast-grep/ast-grep#installation).

``` bash
# install via pip
$ pip install ast-grep-cli

$ which ast-grep
~/.local/bin/ast-grep

$ ast-grep --version
ast-grep 0.39.1

$ ast-grep --help
Search and Rewrite code at large scale using AST pattern.
                    __
        ____ ______/ /_      ____ _________  ____
       / __ `/ ___/ __/_____/ __ `/ ___/ _ \/ __ \
      / /_/ (__  ) /_/_____/ /_/ / /  /  __/ /_/ /
      \__,_/____/\__/      \__, /_/   \___/ .___/
                          /____/         /_/


Usage: ast-grep [OPTIONS] <COMMAND>

Commands:
  run          Run one time search or rewrite in command line. (default command)
  scan         Scan and rewrite code by configuration
  test         Test ast-grep rules
  new          Create new ast-grep project or items like rules/tests
  lsp          Start language server
  completions  Generate shell completion script
  docs         Generate rule docs for current configuration. (Not Implemented Yet)
  help         Print this message or the help of the given subcommand(s)

Options:
  -c, --config <CONFIG_FILE>
          Path to ast-grep root config, default is sgconfig.yml

  -h, --help
          Print help (see a summary with '-h')

  -V, --version
          Print version
```

> Note: Use `sg` on Linux
>
> Linux has a default command `sg` for `setgroups`. You can use the full command name `ast-grep` instead of `sg`. You can also use shorter alias if you want by alias `sg=ast-grep`.


Optionally, you can grab `TypeScript` source code if you want to follow the tutorial. Or you can apply the magic to your own code.

``` bash
git clone https://github.com/microsoft/TypeScript --depth 1
```

## Pattern

Then search the occurrence of looking up a method from a nested structure. `ast-grep` uses **pattern** to find similar code. Think it as the pattern in our old-friend `grep` but it matches AST node instead of text. We can write pattern as if write ordinary code. It will match all code that has the same syntactical structure.

For example, the following pattern code

``` js
obj.val && obj.val()
```

will match all the following code, regardless of white spaces or new lines.

``` js
obj.val && obj.val() // verbatim match, of course
obj.val    &&     obj.val() // this matches, too

// this matches as well!
const result = obj.val &&
   obj.val()
```

Matching based exactly on AST is cool, but we certainly want to use flexible pattern to match code with infinite possibility. We can use **meta variable** to match any single AST node. Meta variable begins with `$` sign with upper case letters following, e.g. `$METAVAR`. **Think it as REGEX dot `.`, except it is not textual**.

We can write this pattern to find all property checking code.

``` js
$PROP && $PROP()
```

It is a valid `ast-grep` pattern! We can use it in command line! Use `pattern` argument to specify our target. Optionally, we can use `lang` to tell `ast-grep` our target code language.

``` bash
# Full Command
ast-grep --pattern '$PROP && $PROP()' --lang ts TypeScript/src

# Short Form
ast-grep -p '$PROP && $PROP()' -l ts TypeScript/src

# Without Lang
# ast-grep will infer languages based on file extensions
ast-grep -p '$PROP && $PROP()' TypeScript/src
```

![astgrep4](/assets/images/202507/astgrep4.png)

![astgrep5](/assets/images/202507/astgrep5.png)


## Rewrite

Cool? Now we can use this pattern to refactor TypeScript source!

``` bash
# pattern and language argument support short form
ast-grep -p '$PROP && $PROP()' \
   --rewrite '$PROP?.()' \
   --interactive \
   -l ts \
   TypeScript/src
```

`ast-grep` will start an interactive session to let you choose if you want to apply the patch. Press `y` to accept the change!

![astgrep6](/assets/images/202507/astgrep6.png)

That's it! You have refactored TypeScript's repository in minutes. Congratulation!

Hope you enjoy the power of AST editing in plain programming language pattern. Our next step is to know more about the pattern code.


# Pattern Syntax

In this guide we will walk through ast-grep's pattern syntax. The example will be written in `JavaScript`, but the basic principle will apply to other languages as well.

## Pattern Matching

`ast-grep` uses pattern code to construct AST tree and match that against target code. The pattern code can search through the full syntax tree, so pattern can also match nested expression. For example, the pattern `a + 1` can match all the following code.

``` js
const b = a + 1

funcCall(a + 1)

deeplyNested({
  target: a + 1
})
```

> **WARNING**
>
> Pattern code must be valid code that tree-sitter can parse.
>
> [ast-grep playground](https://ast-grep.github.io/playground.html) is a useful tool to confirm pattern is parsed correctly.
>
> If ast-grep fails to parse code as expected, you can try give it more context by using [object-style pattern](https://ast-grep.github.io/reference/rule.html#pattern).


## Meta Variable

It is usually desirable to write a pattern to match dynamic content.

We can use meta variables to match sub expression in pattern.

Meta variables start with the `$` sign, followed by a name composed of upper case letters `A-Z`, underscore `_` or digits `1-9`. `$META_VARIABLE` is a wildcard expression that can match any **single AST node**.

Think it as REGEX dot `.`, except it is not textual.

> **Valid meta variables**
>
> `$META`, `$META_VAR`, `$META_VAR1`, `$_`, `$_123`


> Invalid meta variables
>
> `$invalid`, `$Svalue`, `$123`, `$KEBAB-CASE`，`$`


The pattern `console.log($GREETING)` will match all the following.

``` js
function tryAstGrep() {
  console.log('Hello World')
}

const multiLineExpression =
  console
   .log('Also matched!')
```

But it will not match these.

``` js
// console.log(123) in comment is not matched
'console.log(123) in string' // is not matched as well
console.log() // mismatch argument
console.log(a, b) // too many arguments
```

**Note**, one meta variable `$MATCH` will match **one single AST node**, so the last two `console.log` calls do not match the pattern. Let's see how we can match **multiple AST nodes**.


## Multi Meta Variable

We can use `$$$` to match **zero or more AST nodes**, including function arguments, parameters or statements. These variables can also be named, for example: `console.log($$$ARGS)`.


### Function Arguments

For example, `console.log($$$)` can match

``` js
console.log()                       // matches zero AST node
console.log('hello world')          // matches one node
console.log('debug: ', key, value)  // matches multiple nodes
console.log(...args)                // it also matches spread
```

### Function Parameters

`function $FUNC($$$ARGS) { $$$ }` will match

``` js
function foo(bar) {
  return bar
}

function noop() {}

function add(a, b, c) {
  return a + b + c
}
```

![astgrep7](/assets/images/202507/astgrep7.png)


## Meta Variable Capturing

Meta variable is also similar to [capture group](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Regular_Expressions/Groups_and_Backreferences) in regular expression. You can reuse same name meta variables to find previously occurred AST nodes.

For example, the pattern `$A == $A` will have the following result.

``` js
// will match these patterns
a == a
1 + 1 == 1 + 1
// but will not match these
a == b
1 + 1 == 2
```

### Non Capturing Match

You can also suppress meta variable capturing. All meta variables with name starting with underscore `_` will not be captured.

``` js
// Given this pattern

$_FUNC($_FUNC)

// it will match all function call with one argument or spread call
test(a)
testFunc(1 + 1)
testFunc(...args)
```

> **Note** in the example above, even if two meta variables have the same name `$_FUNC`, each occurrence of `$_FUNC` can match different content because they are not captured.

**Why use non-capturing match?**

This is a useful trick to micro-optimize pattern matching speed, since we don't need to create a [HashMap](https://doc.rust-lang.org/stable/std/collections/struct.HashMap.html) for bookkeeping.


### Capture Unnamed Nodes

A meta variable pattern `$META` will capture named nodes by default. To capture unnamed nodes, you can use double dollar sign `$$VAR`.

Namedness is an advanced topic in [Tree-sitter](https://tree-sitter.github.io/tree-sitter/using-parsers#named-vs-anonymous-nodes). You can read this [in-depth guide](https://ast-grep.github.io/advanced/core-concepts.html) for more background.


## More Powerful Rule

Pattern is a fast and easy way to match code. But it is not as powerful as [rule](https://ast-grep.github.io/guide/rule-config.html#rule-file) which can match code with more [precise selector](https://ast-grep.github.io/guide/rule-config/atomic-rule.html#kind) or [more context](https://ast-grep.github.io/guide/rule-config/relational-rule.html).

> **Pro Tip**
>
> Pattern can also be an object instead of string in YAML rule.
>
> It is very useful to avoid ambiguity in code snippet. See [here](https://ast-grep.github.io/guide/rule-config/atomic-rule.html#pattern) for more details.
>
> Also see our FAQ for more [guidance](https://ast-grep.github.io/advanced/faq.html) on writing patterns.



# Rule Essentials

Now you have learnt the basic of ast-grep's pattern syntax and searching. Pattern is a handy feature for simple search. But it is not expressive enough for more complicated cases.

`ast-grep` provides a more sophisticated way to find your code: **Rule**.

**Rules** are like [CSS selectors](https://www.w3schools.com/cssref/css_selectors.php) that can compose together to filter AST nodes based on certain criteria.

## A Minimal Example

A minimal ast-grep rule looks like this.

``` yaml
id: no-await-in-promise-all
language: TypeScript
rule:
  pattern: Promise.all($A)
  has:
    pattern: await $_
    stopBy: end
```

The `TypeScript` rule, `no-await-in-promise-all`, will find `Promise.all` that has `await` expression in it.

It is [suboptimal](https://github.com/hugo-vrijswijk/eslint-plugin-no-await-in-promise/) because `Promise.all` will be called [only after](https://twitter.com/hd_nvim/status/1560108625460355073) the awaited Promise resolves first.

Let's walk through the main fields in this configuration.

* `id` is a unique short string for the rule.

* `language` is the programming language that the rule is intended to check. It specifies what files will be checked against this rule, based on the file extensions. See the list of [supported languages](https://ast-grep.github.io/reference/languages.html).

* `rule` is the most interesting part of ast-grep's configuration. It accepts a `rule object` and defines how the rule behaves and what code will be matched. You can learn how to write rule in the [detailed guide](https://ast-grep.github.io/guide/rule-config/atomic-rule.html).


## Run the Rule

There are several ways to run the rule. We will illustrate several `ast-grep` features here.

### ast-grep scan --rule

The `scan` subcommand of ast-grep CLI can run one rule at a time.

To do so, you need to save the rule above in a file on the disk, say `no-await-in-promise-all.yml`. Then you can run the following command to scan your codebase. In the example below, we are scanning a `test.ts` file.


``` bash
ast-grep scan --rule no-await-in-promise-all.yml test.ts
```

``` ts
await Promise.all([
  await foo(),
])
```

### ast-grep scan --inline-rules

You can also run the rule directly from the command line without saving the rule to a file. The `--inline-rules` option is useful for ad-hoc search or calling `ast-grep` from another program.

``` bash
ast-grep scan --inline-rules '
id: no-await-in-promise-all
language: TypeScript
rule:
  pattern: Promise.all($A)
  has:
    pattern: await $_
    stopBy: end
' test.ts
```

### Online Playground

`ast-grep` provides an [online playground](https://ast-grep.github.io/playground.html#eyJtb2RlIjoiQ29uZmlnIiwibGFuZyI6ImphdmFzY3JpcHQiLCJxdWVyeSI6IlByb21pc2UuYWxsKCRBKSIsInJld3JpdGUiOiIiLCJjb25maWciOiJpZDogbm8tYXdhaXQtaW4tcHJvbWlzZS1hbGxcbmxhbmd1YWdlOiBUeXBlU2NyaXB0XG5ydWxlOlxuICBwYXR0ZXJuOiBQcm9taXNlLmFsbCgkQSlcbiAgaGFzOlxuICAgIHBhdHRlcm46IGF3YWl0ICRfXG4gICAgc3RvcEJ5OiBlbmQiLCJzb3VyY2UiOiJQcm9taXNlLmFsbChbXG4gIGF3YWl0IFByb21pc2UucmVzb2x2ZSgxMjMpXG5dKSJ9) to test your rule.

You can paste the rule configuration into the playground and see the matched code. The playground also has a share button that generates a link to share the rule with others.


## Rule Object

> **Rule object** is the core concept of ast-grep's rule system and every other features are built on top of it.

Below is the full list of fields in a rule object. Every rule field is optional and can be omitted but at least one field should be present in a rule. A node will match a rule if and only if it satisfies all fields in the rule object.

The equivalent rule object interface in `TypeScript` is also provided for reference.

``` yaml
rule:
  # atomic rule
  pattern: 'search.pattern'
  kind: 'tree_sitter_node_kind'
  regex: 'rust|regex'
  # relational rule
  inside: { pattern: 'sub.rule' }
  has: { kind: 'sub_rule' }
  follows: { regex: 'can|use|any' }
  precedes: { kind: 'multi_keys', pattern: 'in.sub' }
  # composite rule
  all: [ {pattern: 'match.all'}, {kind: 'match_all'} ]
  any: [ {pattern: 'match.any'}, {kind: 'match_any'} ]
  not: { pattern: 'not.this' }
  matches: 'utility-rule'
```

``` ts
interface RuleObject {
  // atomic rule
  pattern?: string | Pattern
  kind?: string
  regex?: string
  // relational rule
  inside?: RuleObject & Relation
  has?: RuleObject & Relation
  follows?: RuleObject & Relation
  precedes?: RuleObject & Relation
  // composite rule
  all?: RuleObject[]
  any?: RuleObject[]
  not?: RuleObject
  matches?: string
}

// See Atomic rule for explanation
interface Pattern {
  context: string
  selector: string
  strictness?: Strictness
}

// See https://ast-grep.github.io/advanced/match-algorithm.html
type Strictness =
  | 'cst'
  | 'smart'
  | 'ast'
  | 'relaxed'
  | 'signature'

// See Relation rule for explanation
interface Relation {
  stopBy?: 'neighbor' | 'end' | RuleObject
  field?: string
}
```


A node must **satisfies all fields** in the rule object to be considered as a match. So the rule object can be seen as an abbreviated and **unordered** `all` rule.


> **Rule object is unordered!!**
>
> Unordered rule object means that certain rules may be applied before others, even if they appear later in the YAML. Whether a node matches or not may depend on the order of rule being applied, especially when using `has`/`inside` rules.
>
> If a rule object does not work, you can try using `all` rule to specify the order of rules. See [FAQ](https://ast-grep.github.io/advanced/faq.html#why-is-rule-matching-order-sensitive) for more details.


## Three Rule Categories

To summarize the rule object fields above, we have three categories of rules:

* **Atomic Rule**: the most basic rule that checks if AST nodes matches.
* **Relational Rule**: rules that check if a node is surrounded by another node.
* **Composite Rule**: rules that combine sub-rules together using logical operators.

**These three categories of rules can be composed together to create more complex rules.**

The rule object is inspired by the CSS selectors but with more composability and expressiveness. Think about how selectors in CSS works can help you understand the rule object!

> **TIP**
>
> Don't be daunted! Learn more about how to write a rule in our [detailed guide](https://ast-grep.github.io/guide/rule-config/atomic-rule.html).


## Target Node

Every rule configuration will have **one single root** `rule`. The root rule will have only one AST node in one match. The matched node is called target node. During scanning and rewriting, ast-grep will produce multiple matches to report all AST nodes that satisfies the `rule` condition as matched instances.

Though one rule match only have one AST node as matched, we can have more auxiliary nodes to display context or to perform rewrite. We will cover how rules work in details in the next page.

But for a quick primer, a rule can have a pattern and we can extract meta variables from the matched node.

For example, the rule below will match the `console.log('Hello World')`.

``` yaml
rule:
  pattern: console.log($GREET)
```


And we can get `$GREET` set to `'Hello World'`.

## language specifies rule interpretation

The `language` field in the rule configuration will specify how the rule is interpreted. For example, with `language: TypeScript`, the rule pattern `'hello world'` is parsed as TypeScript string literal. However, the rule will have a parsing error in languages like `C`/`Java`/`Rust` because single quote is used for character literal and double quote should be used for string.


# [Rule Object Reference](https://ast-grep.github.io/reference/rule.html)

* **Atomic rules** are the most basic rules to match AST nodes.
* **Relational rules** filter matched target according to their position relative to other nodes.
* **Composite rules** use logic operation `all`/`any`/`not` to compose the above rules to larger rules.

> All of these **keys** are optional. However, at least one of them must be present and **positive**.

A rule is called **positive** if it only matches nodes with specific kinds. For example, a `kind` rule is positive because it only matches nodes with the kind specified by itself. A `pattern` rule is positive because the pattern itself has a kind and the matching node must have the same kind. A `regex` rule is not positive though because it matches any node as long as its text satisfies the regex.






# [Atomic Rule](https://ast-grep.github.io/guide/rule-config/atomic-rule.html)

`ast-grep` has three categories of rules. Let's start with the most basic one: atomic rule.

Atomic rule defines the most basic matching rule that determines whether one syntax node matches the rule or not. There are five kinds of atomic rule: `pattern`, `kind`, `regex`, `nthChild` and `range`.


## pattern

Pattern will match one single syntax node according to the [pattern syntax](https://ast-grep.github.io/guide/pattern-syntax.html).

``` yaml
rule:
  pattern: console.log($GREETING)
```

The above rule will match code like `console.log('Hello World')`.

By default, a string `pattern` is parsed and matched as a whole.

## Pattern Object

It is not always possible to select certain code with **a simple string pattern**. A pattern code can be invalid, incomplete or ambiguous for the parser since it lacks context.

For example, to select class field in JavaScript, writing `$FIELD = $INIT` will not work because it will be parsed as `assignment_expression`. See [playground](https://ast-grep.github.io/playground.html#eyJtb2RlIjoiUGF0Y2giLCJsYW5nIjoiamF2YXNjcmlwdCIsInF1ZXJ5IjoiJEZJRUxEID0gJElOSVQiLCJyZXdyaXRlIjoiRGVidWcuYXNzZXJ0IiwiY29uZmlnIjoicnVsZTpcbiAgcGF0dGVybjogXG4gICAgY29udGV4dDogJ3sgJE06ICgkJCRBKSA9PiAkTUFUQ0ggfSdcbiAgICBzZWxlY3RvcjogcGFpclxuIiwic291cmNlIjoiYSA9IDEyM1xuY2xhc3MgQSB7XG4gIGEgPSAxMjNcbn0ifQ==).


![astgrep8](/assets/images/202507/astgrep8.png)

We can also use an **object** to **specify a sub-syntax node** to match within a larger context. It consists of an object with three properties: `context`, `selector` and `strictness`.

1. `context` (required): defines the surrounding code that helps to resolve any ambiguity in the syntax.
2. `selector` (optional): defines the sub-syntax node kind that is the actual matcher of the pattern.
3. `strictness` (optional): defines how strictly pattern will match against nodes.

Let's see how **pattern object** can solve the ambiguity(模棱两可) in the class field example above.

The **pattern object** below instructs ast-grep to select the `field_definition` node as the pattern target.

``` yaml
pattern:
  selector: field_definition
  context: class A { $FIELD = $INIT }
```

`ast-grep` works like this:


1. First, the code in `context`, `class A { $FIELD = $INIT }`, is parsed as a class declaration.
2. Then, it looks for the `field_definition` node, specified by selector, in the parsed tree.
3. The selected `$FIELD = $INIT` is matched against code as the pattern.

In this way, the pattern is parsed as `field_definition` instead of `assignment_expression`. See [playground](https://ast-grep.github.io/playground.html#eyJtb2RlIjoiQ29uZmlnIiwibGFuZyI6ImphdmFzY3JpcHQiLCJxdWVyeSI6IiRGSUVMRCA9ICRJTklUIiwicmV3cml0ZSI6IkRlYnVnLmFzc2VydCIsImNvbmZpZyI6InJ1bGU6XG4gIHBhdHRlcm46XG4gICAgc2VsZWN0b3I6IGZpZWxkX2RlZmluaXRpb25cbiAgICBjb250ZXh0OiBjbGFzcyBBIHsgJEZJRUxEID0gJElOSVQgfVxuIiwic291cmNlIjoiYSA9IDEyM1xuY2xhc3MgQSB7XG4gIGEgPSAxMjNcbn0ifQ==) in action.

``` yaml
rule:
  pattern:
    selector: field_definition
    context: class A { $FIELD = $INIT }
```

![astgrep9](/assets/images/202507/astgrep9.png)


Other examples are [function call in Go](https://github.com/ast-grep/ast-grep/issues/646) and [function parameter in Rust](https://github.com/ast-grep/ast-grep/issues/648).



## strictness

You can also use pattern object to control the matching strategy with `strictness` field.

By default, ast-grep uses a smart strategy to match pattern against the AST node. All nodes in the pattern must be matched, but it will skip unnamed nodes in target code.

For the definition of **named** and **unnamed** nodes, please refer to the [core concepts](https://ast-grep.github.io/advanced/core-concepts.html) doc.

For example, the following pattern `function $A() {}` will match both plain function and async function in JavaScript. See [playground](https://ast-grep.github.io/playground.html#eyJtb2RlIjoiUGF0Y2giLCJsYW5nIjoiamF2YXNjcmlwdCIsInF1ZXJ5IjoiZnVuY3Rpb24gJEEoKSB7fSIsInJld3JpdGUiOiJEZWJ1Zy5hc3NlcnQiLCJjb25maWciOiJydWxlOlxuICBwYXR0ZXJuOiBcbiAgICBjb250ZXh0OiAneyAkTTogKCQkJEEpID0+ICRNQVRDSCB9J1xuICAgIHNlbGVjdG9yOiBwYWlyXG4iLCJzb3VyY2UiOiJmdW5jdGlvbiBhKCkge31cbmFzeW5jIGZ1bmN0aW9uIGEoKSB7fSJ9)


``` js
// function $A() {}
function foo() {}    // matched
async function bar() {} // matched
```

![astgrep10](/assets/images/202507/astgrep10.png)

This is because the keyword `async` is an unnamed node in the AST, so the `async` in the code to search is skipped. As long as `function`, `$A` and `{}` are matched, the pattern is considered matched.

However, this is not always the desired behavior. `ast-grep` provides `strictness` to control the matching strategy. At the moment, it provides these options, ordered from the most strict to the least strict:

* `cst`: All nodes in the pattern and target code must be matched. No node is skipped.
* `smart`: All nodes in the pattern must be matched, but it will skip unnamed nodes in target code. This is the default behavior.
* `ast`: Only named AST nodes in both pattern and target code are matched. All unnamed nodes are skipped.
* `relaxed`: Named AST nodes in both pattern and target code are matched. Comments and unnamed nodes are ignored.
* `signature`: Only named AST nodes' kinds are matched. Comments, unnamed nodes and text are ignored.

> **Deep Dive and More Examples**
>
> `strictness` is an advanced feature that you may not need in most cases.
>
> If you are interested in more examples and details, please refer to the [deep dive](https://ast-grep.github.io/advanced/match-algorithm.html) doc on ast-grep's match algorithm.


## kind

Sometimes it is not easy to write a pattern because it is hard to construct the valid syntax.

For example, if we want to match class property declaration in JavaScript like `class A { a = 1 }`, writing `a = 1` will not match the property because it is parsed as assigning to a variable.

Instead, we can use `kind` to specify the AST node type defined in [tree-sitter parser](https://tree-sitter.github.io/tree-sitter/using-parsers#named-vs-anonymous-nodes).

`kind` rule accepts the tree-sitter node's name, like `if_statement` and `expression`. You can refer to [ast-grep playground](https://ast-grep.github.io/playground.html) for relevant `kind` names.

Back to our example, we can look up class property's kind from the playground.

``` yaml
rule:
  kind: field_definition
```

It will match the following code successfully ([playground link](https://ast-grep.github.io/playground.html#eyJtb2RlIjoiQ29uZmlnIiwibGFuZyI6ImphdmFzY3JpcHQiLCJxdWVyeSI6ImEgPSAxMjMiLCJyZXdyaXRlIjoibG9nZ2VyLmxvZygkTUFUQ0gpIiwiY29uZmlnIjoiIyBDb25maWd1cmUgUnVsZSBpbiBZQU1MXG5ydWxlOlxuICBraW5kOiBmaWVsZF9kZWZpbml0aW9uIiwic291cmNlIjoiY2xhc3MgVGVzdCB7XG4gIGEgPSAxMjNcbn0ifQ==)).

``` js
class Test {
  a = 123 // match this line
}
```

![astgrep11](/assets/images/202507/astgrep11.png)

Here are some situations that you can effectively use `kind`:

1. Pattern code is ambiguous to parse, e.g. `{}` in JavaScript can be either object or code block.
2. It is too hard to enumerate all patterns of an AST kind node, e.g. matching all Java/TypeScript class declaration will need including all modifiers, generics, `extends` and `implements`.
3. Patterns only appear within specific context, e.g. the class property definition.


> `kind` + `pattern` is different from pattern object
>
> You may want to use `kind` to change how `pattern` is parsed. However, ast-grep rules are independent of each other.
>
> To change the parsing behavior of `pattern`, you should use pattern object with `context` and `selector` field. See [this FAQ](https://ast-grep.github.io/advanced/faq.html#kind-and-pattern-rules-are-not-working-together-why).



## regex

The `regex` atomic rule will match the AST node by its text against a Rust regular expression.

``` yaml
rule:
  regex: "\w+"
```

> **TIP**
>
> The regular expression is written in [Rust syntax](https://docs.rs/regex/latest/regex/), not the popular [PCRE like syntax](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Regular_Expressions). So some features are not available like arbitrary look-ahead and back references.


You should almost always combine `regex` with other atomic rules to make sure the regular expression is applied to the correct AST node. **Regex matching is quite expensive and cannot be optimized based on AST node kinds**. While `kind` and `pattern` rules can be only applied to nodes with specific `kind_id` for optimized performance.


## nthChild

`nthChild` is a rule to find nodes based on their indexes in the parent node's children list. In other words, it selects nodes based on their position among all sibling nodes within a parent node. It is very helpful in finding nodes without children or nodes appearing in specific positions.

`nthChild` is heavily inspired by CSS's [nth-child pseudo-class](https://developer.mozilla.org/en-US/docs/Web/CSS/:nth-child), and it accepts similar forms of arguments.

``` yaml
# a number to match the exact nth child
nthChild: 3

# An+B style string to match position based on formula
nthChild: 2n+1

# object style nthChild rule
nthChild:
  # accepts number or An+B style string
  position: 2n+1
  # optional, count index from the end of sibling list
  reverse: true # default is false
  # optional, filter the sibling node list based on rule
  ofRule:
    kind: function_declaration # accepts ast-grep rule
```

> **TIP**
>
> nthChild's index is 1-based, not 0-based, as in the CSS selector.
> nthChild's node list only includes named nodes, not unnamed nodes.

The following rule will match the second number in the JavaScript array.

``` yaml
rule:
  kind: number
  nthChild: 2
```

It will match the following code:

``` js
const arr = [ 1, 2, 3, ]
            //   |- match this number
```

![astgrep12](/assets/images/202507/astgrep12.png)


## range

`range` is a rule to match nodes based on their position in the source code. It is useful when you want to integrate external tools like compilers or type checkers with ast-grep. External tools can provide the range information of the interested node, and ast-grep can use it to rewrite the code.

`range` rule accepts a range object with `start` and `end` fields. Each field is an object with `line` and `column` fields.

``` yaml
rule:
  range:
    start:
      line: 0
      column: 0
    end:
      line: 1
      column: 5
```

The above example will match an AST node having the first three characters of the first line like `foo` in `foo.bar()`.

`line` and `column` are 0-based and character-wise, and the `start` is inclusive while the `end` is exclusive.


## Tips for Writing Rules

**Since one rule will have only one AST node in one match, it is recommended to first write the atomic rule that matches the desired node**.

Suppose we want to write a rule which finds functions without a return type. For example, this code would trigger an error:

``` ts
const foo = () => {
	return 1;
}
```

The first step to compose a rule is to find the target. In this case, we can first use kind: `arrow_function` to find function node. Then we can use other rules to filter candidate nodes that does have return type.

Another trick to write cleaner rule is to use sub-rules as fields. Please refer to [composite rule](https://ast-grep.github.io/guide/rule-config/composite-rule.html#combine-different-rules-as-fields) for more details.



# [Relational Rules](https://ast-grep.github.io/guide/rule-config/relational-rule.html)

[Atomic rule](https://ast-grep.github.io/guide/rule-config/atomic-rule.html) **can only match the target node directly**. But sometimes we want to match a node **based on its surrounding nodes**. For example, we want to find `await` expression inside a `for` loop.

**Relational rules** are powerful operators that can filter the target nodes based on their surrounding nodes.

`ast-grep` now supports **four kinds of relational rules**: `inside`, `has`, `follows`, and `precedes`.

All four relational rules **accept a sub rule object** as their value. The sub rule will match the surrounding node while the relational rule itself will match the target node.



# [Composite Rule](https://ast-grep.github.io/guide/rule-config/composite-rule.html)

**Composite rule** can accept another rule or a list of rules recursively. It provides a way to **compose atomic rules into a bigger rule** for more complex matching.

Below are the **four composite rule** operators available in `ast-grep`: `all`, `any`, `not`, and `matches`.


# [Examples](https://ast-grep.github.io/catalog/)


# [Rule Cheat Sheet](https://ast-grep.github.io/cheatsheet/rule.html)

This cheat sheet provides a concise overview of ast-grep's rule object configuration, covering Atomic, Relational, and Composite rules, along with notes on Utility rules. It's designed as a handy reference for common usage.

# [Config Cheat Sheet](https://ast-grep.github.io/cheatsheet/yaml.html)

This cheat sheet provides a concise overview of ast-grep's linter rule YAML configuration. It's designed as a handy reference for common usage.





# [Frequently Asked Questions](https://ast-grep.github.io/advanced/faq.html)

https://ast-grep.github.io/advanced/faq.html


# [Tree-sitter](https://tree-sitter.github.io/tree-sitter/index.html)


**Tree-sitter** is a parser generator tool and an incremental parsing library. It can build a concrete syntax tree for a source file and efficiently update the syntax tree as the source file is edited. Tree-sitter aims to be:

* **General** enough to parse any programming language
* **Fast** enough to parse on every keystroke in a text editor
* **Robust** enough to provide useful results even in the presence of syntax errors
* **Dependency-free** so that the runtime library (which is written in [pure C11](https://github.com/tree-sitter/tree-sitter/tree/master/lib)) can be embedded in any application




# Issue

## [[feature] Parse multiple pieces of code (same language) one time #448](https://github.com/ast-grep/ast-grep/issues/448)

Q: But I don't know how I can do it and what's the best practice to leverage ast-grep(or Rust)'s powerful parallel mechanism.

A: this feature has been supported by `parseAsync`! [ac78bc0](https://github.com/ast-grep/ast-grep/commit/ac78bc0f75346d9ef1ed7de2622dfd02c439a4a3)

Here is the blog benchmarking parallel parsing of different parsers! [Benchmark TypeScript Parsers: Demystify Rust Tooling Performance](https://dev.to/herrington_darkholme/benchmark-typescript-parsers-demystify-rust-tooling-performance-2go8)





# Refer

* https://ast-grep.github.io/
* https://tree-sitter.github.io/tree-sitter/index.html
* [Rule Object Reference](https://ast-grep.github.io/reference/rule.html)














