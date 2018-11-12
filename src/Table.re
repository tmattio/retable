type column('data) = {
  name: string,
  render: 'data => ReasonReact.reactElement,
};

type config('data) = {
  toId: 'data => string,
  columns: list(column('data)),
};

let column = (name: string, render: 'a => ReasonReact.reactElement) => {
  name,
  render,
};

let textColumn = (name: string, render: 'a => string) => {
  name,
  render: data => render(data) |> ReasonReact.string,
};

let intColumn = (name: string, render: 'a => int) => {
  name,
  render: data => render(data) |> string_of_int |> ReasonReact.string,
};

let floatColumn = (name: string, render: 'a => float) => {
  name,
  render: data => render(data) |> string_of_float |> ReasonReact.string,
};

let viewHeader = columns => {
  let header =
    List.map(
      el => <th key={el.name}> {el.name |> ReasonReact.string} </th>,
      columns,
    );

  header |> Array.of_list |> ReasonReact.array;
};

let viewRow = (config: config('a), rowIndex: int, data: 'a) => {
  let columns =
    List.map(
      el => {
        let renderedData = el.render(data);
        <td key={config.toId(data) ++ "-" ++ el.name}> renderedData </td>;
      },
      config.columns,
    );
  let component = columns |> Array.of_list |> ReasonReact.array;

  <tr key={rowIndex |> string_of_int}> component </tr>;
};

let component = ReasonReact.statelessComponent("Table");

let make =
    (
      ~config: config('a),
      ~data: list('a),
      ~className: option(string)=?,
      _children,
    ) => {
  ...component,
  render: _self => {
    let thead = viewHeader(config.columns);

    let tbody =
      List.mapi((i, el) => viewRow(config, i, el), data)
      |> Array.of_list
      |> ReasonReact.array;

    <table ?className>
      <thead> <tr> thead </tr> </thead>
      <tbody> tbody </tbody>
    </table>;
  },
};