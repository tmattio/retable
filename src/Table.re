type column('data) = {
  name: string,
  render: 'data => ReasonReact.reactElement,
};

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

  let viewHeader = columns => {
    let header =
      List.map(
        el => <th key={el.name}> {el.name |> ReasonReact.string} </th>,
        columns,
      );

    header |> Array.of_list |> ReasonReact.array;
  };

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

  type action =
    | SelectItem(Config.t)
    | SelectAll;

  type state = {
    selectedItems: list(item),
    allItemSelected: bool,
  };

  let component = ReasonReact.reducerComponent("Table");

  let make = (~data: list(item), ~className: option(string)=?, _children) => {
    ...component,

    initialState: () => {selectedItems: [], allItemSelected: false},

    reducer: (action, state) =>
      ReasonReact.(
        switch (action) {
        | SelectAll => NoUpdate
        | SelectItem(item) =>
          let updatedSelectedItems =
            Internal.removeOrAddItem(
              ~item,
              ~getItemID=Config.getItemID,
              ~items=state.selectedItems,
              (),
            );
          Update({...state, selectedItems: updatedSelectedItems});
        }
      ),

    render: ({send, state}) => {
      let thead = viewHeader(Config.columns);
      let tbody =
        List.mapi((i, el) => viewRow(i, el), data)
        |> Array.of_list
        |> ReasonReact.array;

      <table ?className>
        <thead> <tr> thead </tr> </thead>
        <tbody> tbody </tbody>
      </table>;
    },
  };
};