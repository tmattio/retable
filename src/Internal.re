let rec removeOrAddItem =
        (
          ~item: 'a,
          ~getItemID: 'a => string,
          ~items: list('a),
          ~newItems: list('a)=[],
          (),
        ) =>
  switch (items) {
  | [] => [item, ...List.rev(items)]
  | [el, ...rest] when getItemID(el) === getItemID(item) =>
    List.rev_append(newItems, rest)
  | [el, ...rest] =>
    removeOrAddItem(
      ~item,
      ~getItemID,
      ~items=rest,
      ~newItems=[el, ...newItems],
      (),
    )
  };