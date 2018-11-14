type column('data) = {
  name: string,
  render: 'data => ReasonReact.reactElement,
};

type items('data) = list('data);

type itemWithSelection('data) = {
  item: 'data,
  selected: bool,
};

type itemsWithSelection('data) = {
  items: list(itemWithSelection('data)),
  onSelect: list('data) => unit,
  isAllSelected: bool,
};

type tableData('data) =
  | Data(items('data))
  | DataWithSelection(itemsWithSelection('data));

type selectionConfig('data) =
  | NotSelectable
  | Selectable(list('data) => unit)
  | SelectableWithInitialState(list('data) => unit, 'data => bool);

let isAllItemsSelected = (data: list(itemWithSelection('data))): bool =>
  !List.exists(el => el.selected === false, data);

let column =
    (name: string, render: 'a => ReasonReact.reactElement): column('a) => {
  name,
  render,
};

let textColumn = (name: string, render: 'a => string): column('a) => {
  name,
  render: data => render(data) |> ReasonReact.string,
};

let intColumn = (name: string, render: 'a => int): column('a) => {
  name,
  render: data => render(data) |> string_of_int |> ReasonReact.string,
};

let floatColumn = (name: string, render: 'a => float): column('a) => {
  name,
  render: data => render(data) |> string_of_float |> ReasonReact.string,
};

module type Config = {
  type t;
  let getItemID: t => string;
  let columns: list(column(t));
};

module Make = (Config: Config) => {
  type item = Config.t;

  let viewHeader = (columns: list(column(item))) =>
    List.map(
      el => <th key={el.name}> {el.name |> ReasonReact.string} </th>,
      columns,
    );

  let viewSelectableHeader =
      (
        columns: list(column(item)),
        isAllChecked: bool,
        onCheckAll: unit => unit,
      ) => {
    let headerTitles = viewHeader(columns);

    [
      <th className="selection">
        <input
          type_="checkbox"
          checked=isAllChecked
          onChange={_ => onCheckAll()}
        />
      </th>,
      ...headerTitles,
    ];
  };

  let viewColumns = (data: item) =>
    List.map(
      el => {
        let renderedData = el.render(data);
        <td key={Config.getItemID(data) ++ "-" ++ el.name}>
          renderedData
        </td>;
      },
      Config.columns,
    );

  let viewRow = (rowIndex: int, data: item) => {
    let columns =
      List.map(
        el => {
          let renderedData = el.render(data);
          <td key={Config.getItemID(data) ++ "-" ++ el.name}>
            renderedData
          </td>;
        },
        Config.columns,
      );
    let component = columns |> Array.of_list |> ReasonReact.array;
    <tr key={rowIndex |> string_of_int}> component </tr>;
  };

  let viewSelectableRow =
      (rowIndex: int, onCheck: item => unit, data: itemWithSelection(item)) => {
    let columns =
      List.map(
        el => {
          let renderedData = el.render(data.item);
          <td key={Config.getItemID(data.item) ++ "-" ++ el.name}>
            renderedData
          </td>;
        },
        Config.columns,
      );

    let columnsWithSelection = [
      <td className="selection">
        <input
          type_="checkbox"
          checked={data.selected}
          onChange={_ => onCheck(data.item)}
        />
      </td>,
      ...columns,
    ];
    let component = columnsWithSelection |> Array.of_list |> ReasonReact.array;
    <tr key={rowIndex |> string_of_int}> component </tr>;
  };

  type action =
    | CheckItem(Config.t)
    | CheckAll;

  type state = tableData(item);

  let component = ReasonReact.reducerComponent("Table");

  let make =
      (
        ~data as initialItems: list(item),
        ~selectionConfig: selectionConfig(item)=NotSelectable,
        ~className: option(string)=?,
        _children,
      ) => {
    ...component,

    /* TODO(tmattio): Implement state update on receive props */
    willReceiveProps: ({state}) => state,

    initialState: () =>
      switch (selectionConfig) {
      | NotSelectable => Data(initialItems)
      | Selectable(callback) =>
        DataWithSelection({
          items: List.map(el => {item: el, selected: false}, initialItems),
          onSelect: callback,
          isAllSelected: false,
        })
      | SelectableWithInitialState(callback, initFunction) =>
        let itemsWithSelection =
          List.map(
            el => {item: el, selected: initFunction(el)},
            initialItems,
          );

        DataWithSelection({
          items: itemsWithSelection,
          onSelect: callback,
          isAllSelected: isAllItemsSelected(itemsWithSelection),
        });
      },

    reducer: (action, state) => {
      let callOnSelectCallback = _ => ();

      ReasonReact.(
        switch (action, state) {
        | (_, Data(_)) => NoUpdate
        | (CheckAll, DataWithSelection({items, isAllSelected, onSelect})) =>
          let updatedItems =
            List.map(
              (el: itemWithSelection(item)) => {
                ...el,
                selected: !isAllSelected,
              },
              items,
            );

          UpdateWithSideEffects(
            DataWithSelection({
              items: updatedItems,
              isAllSelected: !isAllSelected,
              onSelect,
            }),
            callOnSelectCallback,
          );
        | (CheckItem(item), DataWithSelection({items, onSelect})) =>
          let updatedItems =
            List.map(
              (el: itemWithSelection(item)) =>
                if (Config.getItemID(item) === Config.getItemID(el.item)) {
                  {...el, selected: !el.selected};
                } else {
                  el;
                },
              items,
            );

          UpdateWithSideEffects(
            DataWithSelection({
              items: updatedItems,
              isAllSelected: isAllItemsSelected(updatedItems),
              onSelect,
            }),
            callOnSelectCallback,
          );
        }
      );
    },

    render: ({send, state}) => {
      let thead =
        (
          switch (state) {
          | Data(_) => viewHeader(Config.columns)
          | DataWithSelection({isAllSelected}) =>
            viewSelectableHeader(Config.columns, isAllSelected, _ =>
              send(CheckAll)
            )
          }
        )
        |> Array.of_list
        |> ReasonReact.array;

      let tbody =
        (
          switch (state) {
          | Data(data) => List.mapi((i, el) => viewRow(i, el), data)
          | DataWithSelection({items}) =>
            List.mapi(
              (i, el) =>
                viewSelectableRow(i, item => send(CheckItem(item)), el),
              items,
            )
          }
        )
        |> Array.of_list
        |> ReasonReact.array;

      <table ?className>
        <thead> <tr> thead </tr> </thead>
        <tbody> tbody </tbody>
      </table>;
    },
  };
};