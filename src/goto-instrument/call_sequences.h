/*******************************************************************\

Module: Memory-mapped I/O Instrumentation for Goto Programs

Author: Daniel Kroening

Date: September 2011

\*******************************************************************/

/// \file
/// Memory-mapped I/O Instrumentation for Goto Programs

#ifndef CPROVER_GOTO_INSTRUMENT_CALL_SEQUENCES_H
#define CPROVER_GOTO_INSTRUMENT_CALL_SEQUENCES_H

#include <goto-programs/goto_functions.h>

void show_call_sequences(const goto_functionst &goto_functions);
void check_call_sequence(const goto_functionst &goto_functions);

void list_calls_and_arguments(
  const namespacet &ns,
  const goto_functionst &goto_functions);

#endif // CPROVER_GOTO_INSTRUMENT_CALL_SEQUENCES_H
