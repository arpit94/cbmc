/*******************************************************************\

Module: Bounded Model Checking for ANSI-C + HDL

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

/// \file
/// Bounded Model Checking for ANSI-C + HDL

#ifndef CPROVER_CBMC_CBMC_SOLVERS_H
#define CPROVER_CBMC_CBMC_SOLVERS_H

#include <list>
#include <map>
#include <memory>

#include <util/options.h>

#include <solvers/prop/prop.h>
#include <solvers/prop/prop_conv.h>
#include <solvers/sat/cnf.h>
#include <solvers/sat/satcheck.h>
#include <solvers/prop/aig_prop.h>
#include <solvers/smt1/smt1_dec.h>
#include <solvers/smt2/smt2_dec.h>
#include <langapi/language_ui.h>
#include <goto-symex/symex_target_equation.h>

#include "bv_cbmc.h"

class cbmc_solverst:public messaget
{
public:
  cbmc_solverst(
    const optionst &_options,
    const symbol_tablet &_symbol_table,
    message_handlert &_message_handler):
    messaget(_message_handler),
    options(_options),
    symbol_table(_symbol_table),
    ns(_symbol_table),
    ui(ui_message_handlert::uit::PLAIN)
  {
  }

  // The solver class,
  // which owns a variety of allocated objects.
  class solvert
  {
  public:
    solvert()
    {
    }

    explicit solvert(std::unique_ptr<prop_convt> p):prop_conv_ptr(std::move(p))
    {
    }

    solvert(std::unique_ptr<prop_convt> p1, std::unique_ptr<propt> p2):
      prop_ptr(std::move(p2)),
      prop_conv_ptr(std::move(p1))
    {
    }

    solvert(std::unique_ptr<prop_convt> p1, std::unique_ptr<std::ofstream> p2):
      ofstream_ptr(std::move(p2)),
      prop_conv_ptr(std::move(p1))
    {
    }

    prop_convt &prop_conv() const
    {
      assert(prop_conv_ptr!=nullptr);
      return *prop_conv_ptr;
    }

    propt &prop() const
    {
      assert(prop_ptr!=nullptr);
      return *prop_ptr;
    }

    void set_prop_conv(std::unique_ptr<prop_convt> p)
    {
      prop_conv_ptr=std::move(p);
    }

    void set_prop(std::unique_ptr<propt> p)
    {
      prop_ptr=std::move(p);
    }

    void set_ofstream(std::unique_ptr<std::ofstream> p)
    {
      ofstream_ptr=std::move(p);
    }

    // the objects are deleted in the opposite order they appear below
    std::unique_ptr<std::ofstream> ofstream_ptr;
    std::unique_ptr<propt> prop_ptr;
    std::unique_ptr<prop_convt> prop_conv_ptr;
  };

  // returns a solvert object
  virtual std::unique_ptr<solvert> get_solver()
  {
    if(options.get_bool_option("dimacs"))
      return get_dimacs();
    if(options.get_bool_option("refine"))
      return get_bv_refinement();
    else if(options.get_bool_option("refine-strings"))
      return get_string_refinement();
    if(options.get_bool_option("smt1"))
      return get_smt1(get_smt1_solver_type());
    if(options.get_bool_option("smt2"))
      return get_smt2(get_smt2_solver_type());
    return get_default();
  }

  virtual ~cbmc_solverst()
  {
  }

  void set_ui(language_uit::uit _ui) { ui=_ui; }

protected:
  const optionst &options;
  const symbol_tablet &symbol_table;
  namespacet ns;

  // use gui format
  language_uit::uit ui;

  std::unique_ptr<solvert> get_default();
  std::unique_ptr<solvert> get_dimacs();
  std::unique_ptr<solvert> get_bv_refinement();
  std::unique_ptr<solvert> get_string_refinement();
  std::unique_ptr<solvert> get_smt1(smt1_dect::solvert solver);
  std::unique_ptr<solvert> get_smt2(smt2_dect::solvert solver);

  smt1_dect::solvert get_smt1_solver_type() const;
  smt2_dect::solvert get_smt2_solver_type() const;

  // consistency checks during solver creation
  void no_beautification();
  void no_incremental_check();
};

#endif // CPROVER_CBMC_CBMC_SOLVERS_H
