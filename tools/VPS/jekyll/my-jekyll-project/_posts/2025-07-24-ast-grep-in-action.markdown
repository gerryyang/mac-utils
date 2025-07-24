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


# More Powerful Rule

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








# [Frequently Asked Questions](https://ast-grep.github.io/advanced/faq.html)

https://ast-grep.github.io/advanced/faq.html


# Refer

* https://ast-grep.github.io/














