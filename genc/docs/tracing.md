# CenC Computation Tracing

## Tracing in Python

GenC Python package offers tracing for authoring complex workflows. For example
the following code defines a nested prompt template calls, where the output of
template_2 is fed as input to template_1 along with other arguments:

```
@authoring.traced_computation
def comp(aspect, verb, noun):
  template_1 = authoring.prompt_template_with_parameters[
      'Tell me more about the {aspect} aspect of {activity}.',
      ['aspect', 'activity']]
  template_2 = authoring.prompt_template_with_parameters[
      '{noun} {verb}',
      ['noun', 'verb']]
  return template_1(aspect, template_2(noun, verb))
```

The Python function `comp`, decorated with `@authoring.traced_computation`
utilizes a context to trace the variable dependencies between calls.

This is usually expressed as block expression, which is verbose to write.
Authoring interface keeps it simple for developers.

## Tracing in C++

C++ tracing follows the Python closely

*   computation.h maps to computation.py
*   context.h maps to context.py
*   tracing_context.h maps to tracing_context.py

However, C++ can't support decorators and global context is undesirable. Instead
we inject the context into the traced computation object, resulting in a slight
syntax change:

```
// Instead of defining a global context, context is instantiated locally and
// cleaned up after the function call.
auto local_ctx = std::make_shared<TracingContext>();
auto context_stack = std::make_shared<ContextStack>(local_ctx);

// Local context is passed to variable via constructors.
Computation template_1(CreatePromptTemplateWithParameters(...), context_stack);
Computation template_2(CreatePromptTemplateWithParameters(...), context_stack);

Computation template_2_output = template_2(noun, verb);
Computation result = template_1(activity, template_2_output);

// Instead of relying on decorator, we compile the expression explicitly.
return result.Build()
```
