type president = {
  name: string,
  year: int,
  city: string,
  state: string,
};

module PresidentTable =
  ReTable.Make({
    type t = president;
    let getItemID = president => president.name;
    let columns = [
      ReTable.textColumn("Name", el => el.name),
      ReTable.intColumn("Year", el => el.year),
      ReTable.textColumn("City", el => el.city),
      ReTable.textColumn("State", el => el.state),
    ];
  });

let presidents: list(president) = [
  {
    name: "George Washington",
    year: 1732,
    city: "Westmoreland County",
    state: "Virginia",
  },
  {name: "John Adams", year: 1735, city: "Braintree", state: "Massachusetts"},
  {name: "Thomas Jefferson", year: 1743, city: "Shadwell", state: "Virginia"},
];

type state = list(president);

type action =
  | UpdatePresidents(list(president));

let component = ReasonReact.reducerComponent(__MODULE__);

let make = (_children: array(ReasonReact.reactElement)) => {
  ...component,

  initialState: () => [],

  reducer: (action, _state) =>
    switch (action) {
    | UpdatePresidents(presidents) => ReasonReact.Update(presidents)
    },

  render: ({send}) =>
    <PresidentTable
      data=presidents
      selectionConfig={
        SelectableWithInitialState(
          presidents => send(UpdatePresidents(presidents)),
          president => president.name == "George Washington",
        )
      }
    />,
};