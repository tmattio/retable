type column('data) = {
  name: string,
  render: 'data => ReasonReact.reactElement,
};

type withSelection('data) = {
  item: 'data,
  selected: bool,
};

type selectableData('data) = {
  items: list(withSelection('data)),
  onSelect: list('data) => unit,
};

type tableData('data) =
  | Data(list('data))
  | SelectableData(selectableData('data));

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
      (rowIndex: int, onCheck: item => unit, data: withSelection(item)) => {
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

  type state = {
    items: tableData(item),
    isAllChecked: bool,
  };

  let component = ReasonReact.reducerComponent("Table");

  let make =
      (
        ~data as initialItems: tableData(item),
        ~className: option(string)=?,
        _children,
      ) => {
    ...component,

    initialState: () => {
      items: initialItems,
      isAllChecked:
        switch (initialItems) {
        | Data(_) => false
        | SelectableData({items}) =>
          !List.exists(el => el.selected === false, items)
        },
    },

    reducer: (action, state) => {
      let callOnSelectCallback =
          (self: ReasonReact.self('state, 'retainedProps, 'action)): unit =>
        switch (self.state.items) {
        | SelectableData(data) =>
          let items = List.map(el => el.item, data.items);
          data.onSelect(items);
        | _ => ()
        };

      ReasonReact.(
        switch (action, state.items) {
        | (_, Data(_)) => NoUpdate
        | (CheckAll, SelectableData(data)) =>
          let updatedItems =
            List.map(
              (el: withSelection(item)) => {
                ...el,
                selected: !state.isAllChecked,
              },
              data.items,
            );

          UpdateWithSideEffects(
            {
              items: SelectableData({...data, items: updatedItems}),
              isAllChecked: !state.isAllChecked,
            },
            callOnSelectCallback,
          );
        | (CheckItem(item), SelectableData(data)) =>
          let updatedItems =
            List.map(
              (el: withSelection(item)) =>
                if (Config.getItemID(item) === Config.getItemID(el.item)) {
                  {...el, selected: !el.selected};
                } else {
                  el;
                },
              data.items,
            );

          UpdateWithSideEffects(
            {
              ...state,
              items: SelectableData({...data, items: updatedItems}),
            },
            callOnSelectCallback,
          );
        }
      );
    },

    render: ({send, state}) => {
      let thead =
        (
          switch (state.items) {
          | Data(_) => viewHeader(Config.columns)
          | SelectableData(_) =>
            viewSelectableHeader(Config.columns, state.isAllChecked, _ =>
              send(CheckAll)
            )
          }
        )
        |> Array.of_list
        |> ReasonReact.array;

      let tbody =
        (
          switch (state.items) {
          | Data(data) => List.mapi((i, el) => viewRow(i, el), data)
          | SelectableData({items}) =>
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