import React, { Component } from 'react';
import { connect } from 'react-redux';
import { withTranslation } from 'react-i18next';
import {
    Button,
    Collapse,
    CustomInput,
    Modal,
    ModalBody,
    ModalFooter,
    ModalHeader
} from 'reactstrap';
import {format} from 'date-fns';
import DateRangePicker from 'react-daterange-picker';

import Moment from 'moment';
import { extendMoment } from 'moment-range';


import { addInterval, editInterval } from '../../redux/measures';

import 'react-daterange-picker/dist/css/react-calendar.css'
import './Measures.scss';

const moment = extendMoment(Moment);

const State = Object.freeze({
    NEW: Symbol('new'),
    EDIT: Symbol('edit'),
    CLOSED: Symbol('closed')
})

class DateRangeModal extends Component {

    constructor(props) {
        super(props);
        this.state = {
            range: this.sanitize(this.props.interval)
        };
    }

    sanitize(range) {
        if(!range) {
            return null;
        }
        
        return moment.range(
            new Date(range.start),
            new Date(range.end)
        );
    }

    onSelect(range) {
        this.setState({range});
    }

    select() {
        if (this.props.onSelect) {
            const {start, end} = this.state.range;
            this.props.onSelect({
                ...this.props.interval,
                start: start.toDate().getTime(), 
                end: end.toDate().getTime()
            });
        }
    }

    cancel() {
        if (this.props.onCancel) {
            this.props.onCancel();
        }
    }

    render() {
        const {t} = this.props;
        return (
            <Modal
                isOpen={true}
                modalTransition={{ timeout: 10 }}
                className="date-range"
            >
                <ModalHeader>{t('daterangemodal.header')}</ModalHeader>
                <ModalBody>
                    <DateRangePicker 
                        onSelect={this.onSelect.bind(this)}
                        value={this.state.range}
                        numberOfCalendars={2}
                    />
                </ModalBody>
                <ModalFooter>
                    <Button color="success" disabled={this.state.range === null} onClick={this.select.bind(this)}>{t('select')}</Button>
                    <Button color="danger"  onClick={this.cancel.bind(this)}>{t('cancel')}</Button>
                </ModalFooter>
            </Modal>
        );
    }
}

const TranslatedDateRangeModal = withTranslation()(DateRangeModal);

class Measure extends Component {
    constructor(props) {
        super(props);
        this.state = {
            isActive: this.props.data.intervals > 0,
            interval: null,
            modal: State.CLOSED
        };
    }

    toggle() {
        const state = this.state;
        this.setState({
            isActive: !state.isActive
        });
    }

    toggleModal() {
        const state = this.state;
        this.setState({
            modal: !state.modal
        });
    }

    newInterval() {
        this.setState({
            modal: State.NEW,
            interval: null
        });
    }

    editInterval(interval) {
        this.setState({
            modal: State.EDIT,
            interval
        });
    }

    close() {
        this.setState({
            modal: State.CLOSED,
            interval: null
        });
    }

    onSelect(interval) {
        switch(this.state.modal) {
            case State.NEW:
                this.props.addInterval({
                    measure: this.props.data.id,
                    ...interval
                });
                break;

            case State.EDIT:
                this.props.editInterval({
                    measure: this.props.data.id,
                    ...interval,
                });
                break;

            default:
                break;
        }

        this.close();
    }

    renderModal() {
        return (this.state.modal !== State.CLOSED ? <TranslatedDateRangeModal
            interval={this.state.interval}
            onCancel={() => this.close()}
            onSelect={this.onSelect.bind(this)}
        /> : "");
    }

    render() {
        const m = this.props.data;
        const t = this.props.t;
        const dateformat = t('dateFormat');
        return (
            <div className="measure px-3 mb-2">
                <CustomInput
                    type="switch"
                    id={m.label}
                    value={this.state.isActive}
                    onChange={() => this.toggle()}
                    className="d-inline"
                />
                <span className="h6 ml-0">{t(m.label)}</span>
                {this.state.isActive ? (
                    <Button onClick={() => this.newInterval()} size="sm" color="green" className="ml-2 py-0">{t("new")}</Button>
                ) : (
                        <span></span>
                    )}
                <Collapse isOpen={this.state.isActive} >
                    <div className="intervals">
                        {m.intervals.map((interval, i) => {
                            return (
                                <div className="interval" key={interval.id}>
                                    <span className="h6">{format(interval.start, dateformat)} - {format(interval.end, dateformat)}</span>
                                    <Button 
                                        color="warning" 
                                        onClick={() => this.editInterval(interval)}>
                                            <i className="fa fa-edit"></i>
                                    </Button>
                                </div>
                            );
                        })}
                    </div>
                </Collapse>
                {this.renderModal()}
            </div>
        );
    }
}

const TranslatedMeasure = connect(null, {addInterval, editInterval})(withTranslation()(Measure));

class Measures extends Component {
   
    render() {
        const { t } = this.props;
        return (
            <div className="measures">
                <h4 className="p-2 border-bottom border-secondary">{t('measures.title')}</h4>
                {this.props.measures.map((m, i) => {
                    return <TranslatedMeasure key={i} data={m} />
                })}
            </div>
        )
    }
}

const mapState = (state) => {
    console.log(state);
    return {
        measures: state.measures
    };
}

export default connect(mapState, {})(withTranslation()(Measures))