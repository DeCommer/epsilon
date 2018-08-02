#include <poincare/n_ary_expression_node.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

void NAryExpressionNode::sortOperands(ExpressionOrder order, bool canBeInterrupted) {
  ExpressionReference reference(this);
  for (int i = reference.numberOfChildren()-1; i > 0; i--) {
    bool isSorted = true;
    for (int j = 0; j < reference.numberOfChildren()-1; j++) {
      /* Warning: Matrix operations are not always commutative (ie,
       * multiplication) so we never swap 2 matrices. */
#if MATRIX_EXACT_REDUCING
      if (order(childAtIndex(j), childAtIndex(j+1), canBeInterrupted) > 0 && (!childAtIndex(j)->recursivelyMatches(Expression::IsMatrix) || !childAtIndex(j+1)->recursivelyMatches(Expression::IsMatrix))) {
#else
      if (order(childAtIndex(j), childAtIndex(j+1), canBeInterrupted) > 0) {
#endif
        reference.swapChildren(j, j+1);
        isSorted = false;
      }
    }
    if (isSorted) {
      return;
    }
  }
}

ExpressionReference NAryExpressionNode::squashUnaryHierarchy() {
  ExpressionReference reference(this);
  if (reference.numberOfChildren() == 1) {
    assert(parent() != nullptr);
    ExpressionReference o = reference.childAtIndex(0);
    reference.replaceWith(o);
    return o;
  }
  return reference;
}

// Private

int NAryExpressionNode::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
  int m = this->numberOfChildren();
  int n = e->numberOfChildren();
  for (int i = 1; i <= m; i++) {
    // The NULL node is the least node type.
    if (n < i) {
      return 1;
    }
    int order = SimplificationOrder(childAtIndex(m-i), e->childAtIndex(n-i), canBeInterrupted);
    if (order != 0) {
      return order;
    }
  }
  // The NULL node is the least node type.
  if (n > m) {
    return -1;
  }
  return 0;
}

int NAryExpressionNode::simplificationOrderGreaterType(const ExpressionNode * e, bool canBeInterrupted) const {
  int m = numberOfChildren();
  if (m == 0) {
    return -1;
  }
  /* Compare e to last term of hierarchy. */
  int order = SimplificationOrder(childAtIndex(m-1), e, canBeInterrupted);
  if (order != 0) {
    return order;
  }
  if (m > 1) {
    return 1;
  }
  return 0;
}

}