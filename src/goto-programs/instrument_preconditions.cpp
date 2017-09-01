/*******************************************************************\

Module: Move preconditions of a function
        to the call-site of the function

Author: Daniel Kroening

Date:   September 2017

\*******************************************************************/

#include "instrument_preconditions.h"

std::vector<goto_programt::const_targett> get_preconditions(
  const symbol_exprt &function,
  const goto_functionst &goto_functions)
{
  const irep_idt &identifier=function.get_identifier();

  auto f_it=goto_functions.function_map.find(identifier);
  if(f_it==goto_functions.function_map.end())
    return {};

  const auto &body=f_it->second.body;
  
  std::vector<goto_programt::const_targett> result;
  
  for(auto i_it=body.instructions.begin();
      i_it!=body.instructions.end();
      i_it++)
  {
    if(i_it->is_assert() &&
       i_it->source_location.get_property_class()==ID_precondition)
      result.push_back(i_it);
    else
      break; // preconditions must be at the front
  }
  
  return result;
}

void instrument_preconditions(
  const goto_functionst &goto_functions,
  goto_programt &goto_program)
{
  for(auto it=goto_program.instructions.begin();
      it!=goto_program.instructions.end();
      it++)
  {
    if(it->is_function_call())
    {
      // does the function we call have preconditions?
      const auto &call=to_code_function_call(it->code);
      
      if(call.function().id()==ID_symbol)
      {
        auto preconditions=
          get_preconditions(to_symbol_expr(call.function()),
                            goto_functions);
                            
        source_locationt source_location=it->source_location;
        irep_idt function=it->function;

        // add before the call, with location of the call
        for(const auto &p : preconditions)
        {
          goto_program.insert_before_swap(it);
          it->code=p->code;
          it->function=function;
          it->source_location=source_location;
          it->source_location.set_property_class(ID_precondition_instance);
          it->source_location.set_comment(p->source_location.get_comment());
        }
      }
    }
  }
}

void instrument_preconditions(goto_modelt &goto_model)
{
  // add at call site
  for(auto &f_it : goto_model.goto_functions.function_map)
    instrument_preconditions(
      goto_model.goto_functions,
      f_it.second.body);
}
