---
layout: post
title:  "MCP in Action"
date:   2025-04-13 20:00:00 +0800
categories: 机器学习
---

* Do not remove this line (it will not be displayed)
{:toc}




# [Model Context Protocol](https://modelcontextprotocol.io/introduction) (MCP)

`MCP` is an open protocol that standardizes how applications provide context to LLMs. Think of `MCP` like a `USB-C` port for AI applications. Just as `USB-C` provides a standardized way to connect your devices to various peripherals and accessories, `MCP` provides a standardized way to connect AI models to different data sources and tools.

Model Context Protocol，即模型上下文协议。是由 Anthropic 于 2024 年推出的开放协议，旨在为大语言模型 LLM 与外部数据源、工具之间建立标准化的通信框架。它通过统一的接口打破数据孤岛，让 AI 模型能够安全高效地连接各类资源，类似于 AI 应用领域的 "USB-C 接口"。MCP 是一个标准协议，使 AI 模型与 API 无缝交互，而 AI Agent 是一个自主运行的智能系统，利用 Function Calling 和 MCP 来分析和执行任务，实现特定目标。

例如，如果你有个计算器服务，接入 MCP 后，大模型可以直接调用你的加减乘除功能，而不是生成一段代码让用户去执行。

## What is MCP?


The [Model Context Protocol (MCP)](https://modelcontextprotocol.io/) lets you build servers that expose data and functionality to LLM applications in a secure, standardized way. Think of it like a web API, but specifically designed for LLM interactions. MCP servers can:

* Expose data through **Resources** (think of these sort of like GET endpoints; they are used to load information into the LLM's context)
* Provide functionality through **Tools** (sort of like POST endpoints; they are used to execute code or otherwise produce a side effect)
* Define interaction patterns through **Prompts** (reusable templates for LLM interactions)
* And more!



## Why MCP?

`MCP` helps you build agents and complex workflows on top of LLMs. LLMs frequently need to integrate with data and tools, and `MCP` provides:

* A growing list of pre-built integrations that your LLM can directly plug into
* The flexibility to switch between LLM providers and vendors
* Best practices for securing your data within your infrastructure

* 预建集成库
  + 提供开箱即用的连接器（如Salesforce、MySQL、天气API），类似“应用商店”，减少重复开发成本。

* 供应商无绑定（Vendor Agnostic）
  + 抽象层设计，允许自由切换LLM提供商（如GPT-4 → Claude-3），避免厂商锁定，提升谈判灵活性。

* 安全与隐私
  + 遵循零信任原则，数据流经自有基础设施，支持端到端加密、细粒度权限控制，满足企业合规需求（如GDPR）。


**General architecture**

At its core, MCP follows a client-server architecture where a host application can connect to multiple servers:

![mcp](/assets/images/202503/mcp.png)

* MCP Hosts: Programs like Claude Desktop, IDEs, or AI tools that want to access data through MCP
* MCP Clients: Protocol clients that maintain 1:1 connections with servers
* MCP Servers: Lightweight programs that each expose specific capabilities through the standardized Model Context Protocol
* Local Data Sources: Your computer’s files, databases, and services that MCP servers can securely access
* Remote Services: External systems available over the internet (e.g., through APIs) that MCP servers can connect to

# Core architecture - Understand how MCP connects clients, servers, and LLMs

**The Model Context Protocol** (`MCP`) is built on a flexible, extensible architecture that enables seamless communication between `LLM` applications and integrations.

`MCP` follows a client-server architecture where:

* **Hosts** are LLM applications (like `Claude Desktop` or `IDEs`) that initiate connections
* **Clients** maintain `1:1` connections with servers, inside the host application
* **Servers** provide `context`, `tools`, and `prompts` to clients

![mcp2](/assets/images/202503/mcp2.png)

## Protocol layer

**The protocol layer** handles message framing, request/response linking, and high-level communication patterns.

## Transport layer

**The transport layer** handles the actual communication between clients and servers. MCP supports multiple transport mechanisms:

1. Stdio transport
    + Uses standard input/output for communication
    + Ideal for local processes

2. HTTP with SSE transport
    + Uses Server-Sent Events for server-to-client messages
    + HTTP POST for client-to-server messages

All transports use **JSON-RPC 2.0** to exchange messages. See the specification for detailed information about the Model Context Protocol message format.

## Message types

MCP has these main types of messages:

* **Requests** expect a response from the other side:

``` TypeScript
interface Request {
  method: string;
  params?: { ... };
}
```

* **Results** are successful responses to requests:

``` TypeScript
interface Result {
  [key: string]: unknown;
}
```

* **Errors** indicate that a request failed:

``` TypeScript
interface Error {
  code: number;
  message: string;
  data?: unknown;
}
```

* **Notifications** are one-way messages that don’t expect a response:

``` TypeScript
interface Notification {
  method: string;
  params?: { ... };
}
```

## Connection lifecycle

*** Initialization**

![mcp3](/assets/images/202503/mcp3.png)

*** Message exchange**

After initialization, the following patterns are supported:

1. **Request-Response**: Client or server sends requests, the other responds
2. **Notifications**: Either party sends one-way messages

*** Termination**

Either party can terminate the connection:

1. Clean shutdown via `close()`
2. Transport disconnection
3. Error conditions


## Error handling

MCP defines these standard error codes:

``` go
enum ErrorCode {
  // Standard JSON-RPC error codes
  ParseError = -32700,
  InvalidRequest = -32600,
  MethodNotFound = -32601,
  InvalidParams = -32602,
  InternalError = -32603
}
```

SDKs and applications can define their own error codes above `-32000`.

Errors are propagated through:

* Error responses to requests
* Error events on transports
* Protocol-level error handlers


# [Resources](https://modelcontextprotocol.io/docs/concepts/resources) - Expose data and content from your servers to LLMs

`Resources` are a core primitive in the Model Context Protocol (MCP) that allow servers to expose data and content that can be read by clients and used as context for LLM interactions.

`Resources` represent any kind of data that an MCP server wants to make available to clients. This can include:

* File contents
* Database records
* API responses
* Live system data
* Screenshots and images
* Log files
* And more

Each resource is identified by a unique URI and can contain either text or binary data.

## Resource URIs

Resources are identified using URIs that follow this format:

```
[protocol]://[host]/[path]
```

For example:

```
1. file:///home/user/documents/report.pdf
2. postgres://database/customers/schema
3. screen://localhost/display1
```

The protocol and path structure is defined by the MCP server implementation. Servers can define their own custom URI schemes.

## Resource types

Resources can contain two types of content:

**Text resources**

Text resources contain UTF-8 encoded text data. These are suitable for:

* Source code
* Configuration files
* Log files
* JSON/XML data
* Plain text
​
**Binary resources**

Binary resources contain raw binary data encoded in base64. These are suitable for:

* Images
* PDFs
* Audio files
* Video files
* Other non-text formats
​
# [Prompts](https://modelcontextprotocol.io/docs/concepts/prompts) - Create reusable prompt templates and workflows

Prompts enable servers to define reusable prompt templates and workflows that clients can easily surface to users and LLMs. They provide a powerful way to standardize and share common LLM interactions.

Prompts in MCP are predefined templates that can:

* Accept dynamic arguments
* Include context from resources
* Chain multiple interactions
* Guide specific workflows
* Surface as UI elements (like slash commands)

## Prompt structure

Each prompt is defined with:

``` TypeScript
{
  name: string;              // Unique identifier for the prompt
  description?: string;      // Human-readable description
  arguments?: [              // Optional list of arguments
    {
      name: string;          // Argument identifier
      description?: string;  // Argument description
      required?: boolean;    // Whether argument is required
    }
  ]
}
```

## Discovering prompts

Clients can discover available prompts through the `prompts/list` endpoint:

``` TypeScript
// Request
{
  method: "prompts/list"
}

// Response
{
  prompts: [
    {
      name: "analyze-code",
      description: "Analyze code for potential improvements",
      arguments: [
        {
          name: "language",
          description: "Programming language",
          required: true
        }
      ]
    }
  ]
}
```

## Using prompts

To use a prompt, clients make a `prompts/get` request:

``` TypeScript
// Request
{
  method: "prompts/get",
  params: {
    name: "analyze-code",
    arguments: {
      language: "python"
    }
  }
}

// Response
{
  description: "Analyze Python code for potential improvements",
  messages: [
    {
      role: "user",
      content: {
        type: "text",
        text: "Please analyze the following Python code for potential improvements:\n\n```python\ndef calculate_sum(numbers):\n    total = 0\n    for num in numbers:\n        total = total + num\n    return total\n\nresult = calculate_sum([1, 2, 3, 4, 5])\nprint(result)\n```"
      }
    }
  ]
}
```

## Example implementation

Here’s a complete example of implementing prompts in an MCP server:

``` TypeScript
import { Server } from "@modelcontextprotocol/sdk/server";
import {
  ListPromptsRequestSchema,
  GetPromptRequestSchema
} from "@modelcontextprotocol/sdk/types";

const PROMPTS = {
  "git-commit": {
    name: "git-commit",
    description: "Generate a Git commit message",
    arguments: [
      {
        name: "changes",
        description: "Git diff or description of changes",
        required: true
      }
    ]
  },
  "explain-code": {
    name: "explain-code",
    description: "Explain how code works",
    arguments: [
      {
        name: "code",
        description: "Code to explain",
        required: true
      },
      {
        name: "language",
        description: "Programming language",
        required: false
      }
    ]
  }
};

const server = new Server({
  name: "example-prompts-server",
  version: "1.0.0"
}, {
  capabilities: {
    prompts: {}
  }
});

// List available prompts
server.setRequestHandler(ListPromptsRequestSchema, async () => {
  return {
    prompts: Object.values(PROMPTS)
  };
});

// Get specific prompt
server.setRequestHandler(GetPromptRequestSchema, async (request) => {
  const prompt = PROMPTS[request.params.name];
  if (!prompt) {
    throw new Error(`Prompt not found: ${request.params.name}`);
  }

  if (request.params.name === "git-commit") {
    return {
      messages: [
        {
          role: "user",
          content: {
            type: "text",
            text: `Generate a concise but descriptive commit message for these changes:\n\n${request.params.arguments?.changes}`
          }
        }
      ]
    };
  }

  if (request.params.name === "explain-code") {
    const language = request.params.arguments?.language || "Unknown";
    return {
      messages: [
        {
          role: "user",
          content: {
            type: "text",
            text: `Explain how this ${language} code works:\n\n${request.params.arguments?.code}`
          }
        }
      ]
    };
  }

  throw new Error("Prompt implementation not found");
});
```

## Best practices

When implementing prompts:

1. Use clear, descriptive prompt names
2. Provide detailed descriptions for prompts and arguments
3. Validate all required arguments
4. Handle missing arguments gracefully
5. Consider versioning for prompt templates
6. Cache dynamic content when appropriate
7. Implement error handling
8. Document expected argument formats
9. Consider prompt composability
10. Test prompts with various inputs

# [Tools](https://modelcontextprotocol.io/docs/concepts/tools) - Enable LLMs to perform actions through your server

**Tools** are a powerful primitive in the Model Context Protocol (MCP) that enable servers to expose executable functionality to clients. Through tools, LLMs can interact with external systems, perform computations, and take actions in the real world.

Tools in MCP allow servers to expose executable functions that can be invoked by clients and used by LLMs to perform actions. Key aspects of tools include:

* **Discovery**: Clients can list available tools through the `tools/list` endpoint
* **Invocation**: Tools are called using the `tools/call` endpoint, where servers perform the requested operation and return results
* **Flexibility**: Tools can range from simple calculations to complex API interactions

Like **resources**, tools are identified by unique names and can include descriptions to guide their usage. However, unlike resources, tools represent dynamic operations that can modify state or interact with external systems.

## Tool definition structure

Each tool is defined with the following structure:

``` TypeScript
{
  name: string;          // Unique identifier for the tool
  description?: string;  // Human-readable description
  inputSchema: {         // JSON Schema for the tool's parameters
    type: "object",
    properties: { ... }  // Tool-specific parameters
  },
  annotations?: {        // Optional hints about tool behavior
    title?: string;      // Human-readable title for the tool
    readOnlyHint?: boolean;    // If true, the tool does not modify its environment
    destructiveHint?: boolean; // If true, the tool may perform destructive updates
    idempotentHint?: boolean;  // If true, repeated calls with same args have no additional effect
    openWorldHint?: boolean;   // If true, tool interacts with external entities
  }
}
```

## Implementing tools

Here’s an example of implementing a basic tool in an MCP server:

``` TypeScript
const server = new Server({
  name: "example-server",
  version: "1.0.0"
}, {
  capabilities: {
    tools: {}
  }
});

// Define available tools
server.setRequestHandler(ListToolsRequestSchema, async () => {
  return {
    tools: [{
      name: "calculate_sum",
      description: "Add two numbers together",
      inputSchema: {
        type: "object",
        properties: {
          a: { type: "number" },
          b: { type: "number" }
        },
        required: ["a", "b"]
      }
    }]
  };
});

// Handle tool execution
server.setRequestHandler(CallToolRequestSchema, async (request) => {
  if (request.params.name === "calculate_sum") {
    const { a, b } = request.params.arguments;
    return {
      content: [
        {
          type: "text",
          text: String(a + b)
        }
      ]
    };
  }
  throw new Error("Tool not found");
});
```

## Example tool patterns

Here are some examples of types of tools that a server could provide:

### System operations

Tools that interact with the local system:

``` TypeScript
{
  name: "execute_command",
  description: "Run a shell command",
  inputSchema: {
    type: "object",
    properties: {
      command: { type: "string" },
      args: { type: "array", items: { type: "string" } }
    }
  }
}
```

### API integrations

Tools that wrap external APIs:

``` TypeScript
{
  name: "github_create_issue",
  description: "Create a GitHub issue",
  inputSchema: {
    type: "object",
    properties: {
      title: { type: "string" },
      body: { type: "string" },
      labels: { type: "array", items: { type: "string" } }
    }
  }
}
```

### Data processing

Tools that transform or analyze data:

``` TypeScript
{
  name: "analyze_csv",
  description: "Analyze a CSV file",
  inputSchema: {
    type: "object",
    properties: {
      filepath: { type: "string" },
      operations: {
        type: "array",
        items: {
          enum: ["sum", "average", "count"]
        }
      }
    }
  }
}
```

## Best practices

When implementing tools:

1. Provide clear, descriptive names and descriptions
2. Use detailed JSON Schema definitions for parameters
3. Include examples in tool descriptions to demonstrate how the model should use them
4. Implement proper error handling and validation
5. Use progress reporting for long operations
6. Keep tool operations focused and atomic
7. Document expected return value structures
8. Implement proper timeouts
9. Consider rate limiting for resource-intensive operations
10. Log tool usage for debugging and monitoring






# MCP Server Implementation Example

Here’s a basic example of implementing an **MCP server**:

``` TypeScript
import { Server } from "@modelcontextprotocol/sdk/server/index.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";

const server = new Server({
  name: "example-server",
  version: "1.0.0"
}, {
  capabilities: {
    resources: {}
  }
});

// Handle requests
server.setRequestHandler(ListResourcesRequestSchema, async () => {
  return {
    resources: [
      {
        uri: "example://resource",
        name: "Example Resource"
      }
    ]
  };
});

// Connect transport
const transport = new StdioServerTransport();
await server.connect(transport);
```

# Best practices

## Transport selection

1. Local communication
   + Use stdio transport for local processes
   + Efficient for same-machine communication
   + Simple process management

2. Remote communication
   + Use SSE for scenarios requiring HTTP compatibility
   + Consider security implications including authentication and authorization

## Message handling

1. Request processing
   + Validate inputs thoroughly
   + Use type-safe schemas
   + Handle errors gracefully
   + Implement timeouts

2. Progress reporting
   + Use progress tokens for long operations
   + Report progress incrementally
   + Include total progress when known

3. Error management
   + Use appropriate error codes
   + Include helpful error messages
   + Clean up resources on errors

## Security considerations

1. Transport security
   + Use TLS for remote connections
   + Validate connection origins
   + Implement authentication when needed

2. Message validation
   + Validate all incoming messages
   + Sanitize inputs
   + Check message size limits
   + Verify JSON-RPC format

3. Resource protection
   + Implement access controls
   + Validate resource paths
   + Monitor resource usage
   + Rate limit requests

4. Error handling
   + Don’t leak sensitive information
   + Log security-relevant errors
   + Implement proper cleanup
   + Handle DoS scenarios

## Debugging and monitoring

1. Logging
   + Log protocol events
   + Track message flow
   + Monitor performance
   + Record errors

2. Diagnostics
   + Implement health checks
   + Monitor connection state
   + Track resource usage
   + Profile performance

3. Testing
   + Test different transports
   + Verify error handling
   + Check edge cases
   + Load test servers


# [MCP Go](https://github.com/mark3labs/mcp-go)

A Go implementation of the Model Context Protocol (MCP), enabling seamless integration between LLM applications and external data sources and tools.

``` go
package main

import (
    "context"
    "errors"
    "fmt"

    "github.com/mark3labs/mcp-go/mcp"
    "github.com/mark3labs/mcp-go/server"
)

func main() {
    // Create MCP server
    s := server.NewMCPServer(
        "Demo 🚀",
        "1.0.0",
    )

    // Add tool
    tool := mcp.NewTool("hello_world",
        mcp.WithDescription("Say hello to someone"),
        mcp.WithString("name",
            mcp.Required(),
            mcp.Description("Name of the person to greet"),
        ),
    )

    // Add tool handler
    s.AddTool(tool, helloHandler)

    // Start the stdio server
    if err := server.ServeStdio(s); err != nil {
        fmt.Printf("Server error: %v\n", err)
    }
}

func helloHandler(ctx context.Context, request mcp.CallToolRequest) (*mcp.CallToolResult, error) {
    name, ok := request.Params.Arguments["name"].(string)
    if !ok {
        return nil, errors.New("name must be a string")
    }

    return mcp.NewToolResultText(fmt.Sprintf("Hello, %s!", name)), nil
}
```

That's it!

MCP Go handles all the complex protocol details and server management, so you can focus on building great tools. It aims to be high-level and easy to use.

**Key features:**

* Fast: High-level interface means less code and faster development
* Simple: Build MCP servers with minimal boilerplate
* Complete: MCP Go aims to provide a full implementation of the core MCP specification

# Example

## Local (stdio)

Let's create a simple MCP server that exposes a calculator tool and some data:

``` go
package main

import (
    "context"
    "errors"
    "fmt"

    "github.com/mark3labs/mcp-go/mcp"
    "github.com/mark3labs/mcp-go/server"
)

func main() {
    // Create a new MCP server
    s := server.NewMCPServer(
        "Calculator Demo",
        "1.0.0",
        server.WithResourceCapabilities(true, true),
        server.WithLogging(),
        server.WithRecovery(),
    )

    // Add a calculator tool
    calculatorTool := mcp.NewTool("calculate",
        mcp.WithDescription("Perform basic arithmetic operations"),
        mcp.WithString("operation",
            mcp.Required(),
            mcp.Description("The operation to perform (add, subtract, multiply, divide)"),
            mcp.Enum("add", "subtract", "multiply", "divide"),
        ),
        mcp.WithNumber("x",
            mcp.Required(),
            mcp.Description("First number"),
        ),
        mcp.WithNumber("y",
            mcp.Required(),
            mcp.Description("Second number"),
        ),
    )

    // Add the calculator handler
    s.AddTool(calculatorTool, func(ctx context.Context, request mcp.CallToolRequest) (*mcp.CallToolResult, error) {
        op := request.Params.Arguments["operation"].(string)
        x := request.Params.Arguments["x"].(float64)
        y := request.Params.Arguments["y"].(float64)

        var result float64
        switch op {
        case "add":
            result = x + y
        case "subtract":
            result = x - y
        case "multiply":
            result = x * y
        case "divide":
            if y == 0 {
                return nil, errors.New("Cannot divide by zero")
            }
            result = x / y
        }

        return mcp.NewToolResultText(fmt.Sprintf("%.2f", result)), nil
    })

    // Start the server
    if err := server.ServeStdio(s); err != nil {
        fmt.Printf("Server error: %v\n", err)
    }
}
```

``` bash
go mod init mcp-go-demo
go mod tidy
```

在 Cursor 中添加 MCP server：

![mcp4](/assets/images/202503/mcp4.png)


## Remote (SSE)

https://modelcontextprotocol.io/quickstart/server#python




# Refer

* https://github.com/modelcontextprotocol
* https://modelcontextprotocol.io/introduction
* [Example Clients](https://modelcontextprotocol.io/clients)
* [Example Servers](https://modelcontextprotocol.io/examples)
* https://github.com/punkpeye/awesome-mcp-servers
* https://github.com/mark3labs/mcp-go
* [Build and use your own MCP in Cursor in 5 minutes](https://dev.to/andyrewlee/use-your-own-mcp-on-cursor-in-5-minutes-1ag4)
* https://cursor.directory/mcp/demo-everything
* https://github.com/modelcontextprotocol/servers/tree/HEAD/src/everything
* https://github.com/modelcontextprotocol/quickstart-resources










